#include "includes.h"

#include <array>
#include <cstring>
#include <string>
#include <vector>

#include "lua_debugger.h"
#include "lua_manager.h"
#include "log.h"
#include "g_errors.h"
#include "PAC_info.h"
#include "tech_def.h"
#include "cip_tech_def.h"

namespace
    {
    std::size_t occurrences( const std::string& text,
        const std::string& fragment )
        {
        std::size_t count = 0;
        std::size_t position = 0;
        while ( ( position = text.find( fragment, position ) ) !=
            std::string::npos )
            {
            count++;
            position += fragment.size();
            }
        return count;
        }

    std::string extract_session_id( const std::string& response )
        {
        const std::string marker = R"("session_id":")";
        const auto start = response.find( marker );
        if ( start == std::string::npos ) return {};
        const auto value_start = start + marker.size();
        const auto end = response.find( '"', value_start );
        return response.substr( value_start, end - value_start );
        }

    class debugger_error_owner final : public i_simple_error
        {
        public:
            void set_error_params( saved_params_u_int_4* ) override {}
            const char* get_name() const override { return "TEST_DEVICE"; }
            const char* get_error_description() override
                { return "test device error"; }
            int get_error_id() override { return -1; }
            int get_state() const override { return -1; }
            u_int_4 get_serial_n() const override { return 1; }
            int get_error_type() const override { return 1; }
        };

    class lua_debugger_test : public ::testing::Test
        {
        protected:
            void SetUp() override
                {
                state = lua_open();
                luaL_openlibs( state );
                G_LUA_MANAGER->set_Lua( state );
                G_LUA_DEBUGGER->reset();
                session_id = extract_session_id(
                    raw_request( lua_debugger::CMD_CREATE_SESSION ) );
                ASSERT_FALSE( session_id.empty() );
                }

            void TearDown() override
                {
                G_LUA_DEBUGGER->reset( state );
                G_LUA_MANAGER->free_Lua();
                }

            std::string raw_request( lua_debugger::COMMAND command,
                const std::string& text = {} )
                {
                std::vector<unsigned char> input( text.size() + 1 );
                std::array<unsigned char, 65536> output{};
                input[ 0 ] = command;
                std::memcpy( input.data() + 1, text.data(), text.size() );
                const auto size = lua_debugger::process_service(
                    static_cast<long>( text.size() + 1 ), input.data(),
                    output.data() );
                EXPECT_GT( size, 0 );
                return reinterpret_cast<const char*>( output.data() );
                }

            std::string request( lua_debugger::COMMAND command,
                const std::string& text = {} )
                {
                return raw_request( command, session_id + "\n" + text );
                }

            lua_State* state = nullptr;
            std::string session_id;
        };
    }

TEST_F( lua_debugger_test, creates_and_closes_session )
    {
    EXPECT_EQ( 1u, G_LUA_DEBUGGER->sessions_count() );
    EXPECT_EQ( R"({"ok":true})", request( lua_debugger::CMD_KEEP_ALIVE ) );
    EXPECT_EQ( R"({"ok":true})", request( lua_debugger::CMD_CLOSE_SESSION ) );
    EXPECT_EQ( 0u, G_LUA_DEBUGGER->sessions_count() );
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_KEEP_ALIVE ).find( "expired session" ) );
    }

TEST_F( lua_debugger_test, session_contains_controller_time_anchor )
    {
    const auto response = raw_request( lua_debugger::CMD_CREATE_SESSION );
    EXPECT_NE( std::string::npos,
        response.find( R"("controller_time_unix_ms":)" ) );
    EXPECT_NE( std::string::npos,
        response.find( R"("controller_time_millisec":)" ) );
    }

TEST_F( lua_debugger_test, expires_inactive_session )
    {
    G_LUA_DEBUGGER->expire_sessions_for_test(
        lua_debugger::SESSION_TIMEOUT_MS + 1 );
    EXPECT_EQ( 0u, G_LUA_DEBUGGER->sessions_count() );
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_KEEP_ALIVE ).find( "expired session" ) );
    }

TEST_F( lua_debugger_test, evaluates_lua_expression )
    {
    EXPECT_EQ( R"({"ok":true,"type":"number","value":3})",
        request( lua_debugger::CMD_EVALUATE, "1 + 2" ) );
    EXPECT_EQ( R"({"ok":true,"type":"string","value":"a\"b"})",
        request( lua_debugger::CMD_EVALUATE, R"("a\"b")" ) );

    const auto error = request( lua_debugger::CMD_EVALUATE, "1 +" );
    EXPECT_NE( std::string::npos, error.find( R"("ok":false)" ) );
    EXPECT_NE( std::string::npos, error.find( R"("type":"error")" ) );
    }

TEST_F( lua_debugger_test, executes_lua_chunks_and_restores_stack )
    {
    lua_pushstring( state, "stack sentinel" );
    const int top = lua_gettop( state );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":2})",
        request( lua_debugger::CMD_EXECUTE,
            "remote_value = 1\nremote_value = remote_value + 1\n"
            "return remote_value, 99" ) );
    EXPECT_EQ( R"({"ok":true,"type":"nil","value":null})",
        request( lua_debugger::CMD_EXECUTE, "remote_value = 3" ) );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":3})",
        request( lua_debugger::CMD_EVALUATE, "remote_value" ) );
    for ( const auto& source : { "local =", "error('failed')" } )
        {
        EXPECT_NE( std::string::npos,
            request( lua_debugger::CMD_EXECUTE, source ).find(
                R"("ok":false)" ) );
        EXPECT_EQ( top, lua_gettop( state ) );
        }
    EXPECT_EQ( top, lua_gettop( state ) );
    EXPECT_STREQ( "stack sentinel", lua_tostring( state, -1 ) );
    }

TEST_F( lua_debugger_test, execute_requires_session_and_limits_source_size )
    {
    EXPECT_NE( std::string::npos,
        raw_request( lua_debugger::CMD_EXECUTE, "invalid\nx = 1" ).find(
            "Invalid or expired session" ) );
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_EXECUTE ).find( R"("ok":false)" ) );
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_EXECUTE, std::string( 16385, ' ' ) ).find(
            "too long" ) );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":1})",
        request( lua_debugger::CMD_EXECUTE,
            std::string( 16376, ' ' ) + "return 1" ) );
    }

TEST_F( lua_debugger_test, executes_setter_once_and_rereads_getter )
    {
    ASSERT_EQ( 0, luaL_dostring( state,
        "LINE1V1 = {value=0, calls=0}; "
        "function LINE1V1:get_value() return self.value end; "
        "function LINE1V1:set_value(v) "
        "self.calls=self.calls+1; self.value=math.min(v, 1) end" ) );
    const std::string code =
        "if not (2 == 0 or 2 == 1 or 2 == 2) then "
        "error('Value is not allowed') end\n"
        "do local function apply() LINE1V1:set_value(2) end\n"
        "apply() end\nreturn (LINE1V1:get_value())";
    EXPECT_EQ( R"({"ok":true,"type":"number","value":1})",
        request( lua_debugger::CMD_EXECUTE, code ) );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":1})",
        request( lua_debugger::CMD_EVALUATE, "LINE1V1.calls" ) );
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_EXECUTE,
            "if not (3 == 0 or 3 == 1 or 3 == 2) then "
            "error('Value is not allowed') end\nLINE1V1:set_value(3)" ).find(
                R"("ok":false)" ) );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":1})",
        request( lua_debugger::CMD_EVALUATE, "LINE1V1.calls" ) );
    }

TEST_F( lua_debugger_test, executes_known_controller_commands )
    {
    EXPECT_EQ( R"({"ok":true,"command":301,"result":0,"queued":false})",
        request( lua_debugger::CMD_EXEC_CONTROLLER_COMMAND, "301" ) );
    EXPECT_TRUE( G_PAC_INFO()->is_phoenix_modbus_udp() );
    EXPECT_EQ( R"({"ok":true,"command":302,"result":0,"queued":false})",
        request( lua_debugger::CMD_EXEC_CONTROLLER_COMMAND, "302" ) );
    EXPECT_FALSE( G_PAC_INFO()->is_phoenix_modbus_udp() );
    EXPECT_EQ( R"({"ok":true,"command":102,"result":0,"queued":true})",
        request( lua_debugger::CMD_EXEC_CONTROLLER_COMMAND, "102" ) );
    EXPECT_EQ( R"({"ok":true,"command":0,"result":0,"queued":false})",
        request( lua_debugger::CMD_EXEC_CONTROLLER_COMMAND, "0" ) );
    EXPECT_EQ( R"({"ok":false,"error":"Unknown controller command"})",
        request( lua_debugger::CMD_EXEC_CONTROLLER_COMMAND, "999" ) );
    EXPECT_EQ( R"({"ok":false,"error":"Invalid controller command id"})",
        request( lua_debugger::CMD_EXEC_CONTROLLER_COMMAND, "102abc" ) );
    EXPECT_EQ( R"({"ok":false,"error":"Invalid controller command id"})",
        request( lua_debugger::CMD_EXEC_CONTROLLER_COMMAND ) );
    }

TEST_F( lua_debugger_test, caches_only_chart_value_changes )
    {
    ASSERT_EQ( 0, luaL_dostring( state, "debug_x = 10" ) );
    EXPECT_EQ( R"({"ok":true,"count":2})",
        request( lua_debugger::CMD_SET_CHART_EXPRESSIONS,
            "debug_x\ndebug_x * 2" ) );
    EXPECT_EQ( 2u, G_LUA_DEBUGGER->expressions_count( session_id ) );

    G_LUA_DEBUGGER->evaluate();
    G_LUA_DEBUGGER->evaluate();
    ASSERT_EQ( 0, luaL_dostring( state, "debug_x = 11" ) );
    G_LUA_DEBUGGER->evaluate();

    const auto data = request( lua_debugger::CMD_GET_CHART_DATA );
    EXPECT_NE( std::string::npos,
        data.find( R"("expression":"debug_x")" ) );
    EXPECT_NE( std::string::npos, data.find( R"("value":10)" ) );
    EXPECT_NE( std::string::npos, data.find( R"("value":11)" ) );
    EXPECT_NE( std::string::npos, data.find( R"("value":20)" ) );
    EXPECT_NE( std::string::npos, data.find( R"("value":22)" ) );
    EXPECT_EQ( 4u, occurrences( data, R"("time_ms":)" ) );

    EXPECT_EQ( R"({"ok":true})",
        request( lua_debugger::CMD_CLEAR_CHART_DATA ) );
    const auto cleared = request( lua_debugger::CMD_GET_CHART_DATA );
    EXPECT_EQ( std::string::npos, cleared.find( R"("time_ms":)" ) );
    }

TEST_F( lua_debugger_test, sessions_have_independent_expression_lists )
    {
    ASSERT_EQ( R"({"ok":true,"count":1})",
        request( lua_debugger::CMD_SET_CHART_EXPRESSIONS, "41 + 1" ) );
    const auto second_id = extract_session_id(
        raw_request( lua_debugger::CMD_CREATE_SESSION ) );
    ASSERT_FALSE( second_id.empty() );
    EXPECT_EQ( 2u, G_LUA_DEBUGGER->sessions_count() );
    EXPECT_EQ( 1u, G_LUA_DEBUGGER->expressions_count( session_id ) );
    EXPECT_EQ( 0u, G_LUA_DEBUGGER->expressions_count( second_id ) );
    }

TEST_F( lua_debugger_test, delivers_messages_to_each_current_session )
    {
    G_LUA_DEBUGGER->publish_message( "test", 4, "first message" );

    const auto second_id = extract_session_id(
        raw_request( lua_debugger::CMD_CREATE_SESSION ) );
    ASSERT_FALSE( second_id.empty() );

    const auto first_messages = request( lua_debugger::CMD_GET_MESSAGES );
    EXPECT_NE( std::string::npos, first_messages.find( "first message" ) );
    EXPECT_NE( std::string::npos,
        first_messages.find( R"("source":"test")" ) );

    const auto second_old_messages = raw_request(
        lua_debugger::CMD_GET_MESSAGES, second_id + "\n" );
    EXPECT_EQ( std::string::npos,
        second_old_messages.find( "first message" ) );

    G_LUA_DEBUGGER->publish_message( "test", 3, "shared message" );
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_GET_MESSAGES ).find( "shared message" ) );
    EXPECT_NE( std::string::npos, raw_request(
        lua_debugger::CMD_GET_MESSAGES, second_id + "\n" ).find(
            "shared message" ) );
    }

TEST_F( lua_debugger_test, mirrors_log_messages )
    {
    G_LOG->write_log( i_log::P_INFO, "lua debugger log test" );
    const auto messages = request( lua_debugger::CMD_GET_MESSAGES );
    EXPECT_NE( std::string::npos,
        messages.find( "lua debugger log test" ) );
    EXPECT_NE( std::string::npos, messages.find( R"("source":"log")" ) );
    EXPECT_NE( std::string::npos, messages.find( R"("priority":6)" ) );
    }

TEST_F( lua_debugger_test, poll_combines_chart_and_messages_with_bounded_cache )
    {
    ASSERT_EQ( R"({"ok":true,"count":1})",
        request( lua_debugger::CMD_SET_CHART_EXPRESSIONS, "debug_x" ) );
    for ( int i = 0; i < 70; ++i )
        {
        lua_pushinteger( state, i );
        lua_setglobal( state, "debug_x" );
        G_LUA_DEBUGGER->evaluate();
        G_LUA_DEBUGGER->publish_message( "test", 6, "event" );
        }
    const auto response = request( lua_debugger::CMD_POLL );
    EXPECT_NE( std::string::npos, response.find( R"("events":)" ) );
    EXPECT_NE( std::string::npos, response.find( R"("dropped":6)" ) );
    EXPECT_EQ( 64u, occurrences( response, R"("value":)" ) );
    EXPECT_EQ( 32u, occurrences( response, R"("text":"event")" ) );
    const auto next = request( lua_debugger::CMD_POLL );
    EXPECT_EQ( 1u, occurrences( next, R"("value":)" ) );
    EXPECT_EQ( 32u, occurrences( next, R"("text":"event")" ) );
    }

TEST_F( lua_debugger_test, receives_error_manager_errors )
    {
    debugger_error_owner owner;
    G_ERRORS_MANAGER->clear();
    G_ERRORS_MANAGER->add_error( new simple_error( &owner ) );

    G_ERRORS_MANAGER->evaluate();

    const auto messages = request( lua_debugger::CMD_GET_MESSAGES );
    EXPECT_NE( std::string::npos, messages.find( "test device error" ) );
    EXPECT_NE( std::string::npos,
        messages.find( R"("source":"error_manager")" ) );
    G_ERRORS_MANAGER->clear();
    }

TEST_F( lua_debugger_test, receives_set_err_msg_messages )
    {
    tech_object object( "TEST_OBJECT", 1, 1, "TEST_OBJECT1",
        0, 0, 1, 1, 1, 1 );

    object.set_err_msg( "test object alarm", 0, 0,
        tech_object::ERR_ALARM );

    const auto messages = request( lua_debugger::CMD_GET_MESSAGES );
    EXPECT_NE( std::string::npos, messages.find( "test object alarm" ) );
    EXPECT_NE( std::string::npos,
        messages.find( R"("source":"set_err_msg")" ) );
    }

TEST_F( lua_debugger_test, rejects_invalid_chart_expression_atomically )
    {
    EXPECT_EQ( R"({"ok":true,"count":1})",
        request( lua_debugger::CMD_SET_CHART_EXPRESSIONS, "41 + 1" ) );

    const auto error = request( lua_debugger::CMD_SET_CHART_EXPRESSIONS,
        "valid_name\n1 +" );
    EXPECT_NE( std::string::npos, error.find( R"("ok":false)" ) );
    EXPECT_EQ( 1u, G_LUA_DEBUGGER->expressions_count( session_id ) );
    }

TEST_F( lua_debugger_test, browses_table_and_global_variables )
    {
    ASSERT_EQ( 0, luaL_dostring( state,
        "dbg_num = 42\n"
        "dbg_text = 'hello'\n"
        "dbg_tbl = { a = 1, nested = { b = 2 } }\n" ) );

    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_tbl\n0" );
    EXPECT_NE( std::string::npos, response.find( R"("ok":true)" ) );
    EXPECT_NE( std::string::npos, response.find( R"("type":"table")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"({"name":"a","expression":"dbg_tbl[\"a\"]","type":"number","value":1,"expandable":false,"writable":true})" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"nested","expression":"dbg_tbl[\"nested\"]","type":"table","value":"<table>","expandable":true,"writable":false)" ) );
    EXPECT_NE( std::string::npos, response.find( R"("next_offset":null)" ) );
    EXPECT_NE( std::string::npos, response.find( R"("truncated":false)" ) );

    // A scalar root reports its value and no children.
    const auto scalar = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_num\n0" );
    EXPECT_NE( std::string::npos,
        scalar.find( R"("type":"number","value":42,"entries":[])" ) );
    }

TEST_F( lua_debugger_test, browses_unusual_table_keys )
    {
    ASSERT_EQ( 0, luaL_dostring( state, R"(
        dbg_tbl = {
            ["with space"] = 1,
            ["quo\"te"] = 2,
            ["line\nbreak"] = 3,
            ["\128key"] = 4,
            [true] = 5,
            [1.5] = 6,
            [100] = 7,
        }
    )" ) );
    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_tbl\n0" );
    EXPECT_NE( std::string::npos, response.find( R"("name":"with space")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("expression":"dbg_tbl[\"with space\"]")" ) );
    EXPECT_NE( std::string::npos, response.find( R"("name":"quo\"te")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("expression":"dbg_tbl[\"quo\\\"te\"]")" ) );
    EXPECT_NE( std::string::npos, response.find( R"("name":"line\nbreak")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("expression":"dbg_tbl[\"line\\nbreak\"]")" ) );
    // Bytes that are not valid UTF-8 become decimal escapes in the name,
    // while the generated expression still compiles.
    EXPECT_NE( std::string::npos, response.find( R"("name":"\\128key")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("expression":"dbg_tbl[\"\\128key\"]")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"true","expression":"dbg_tbl[true]")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"1.5","expression":"dbg_tbl[1.5]")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"100","expression":"dbg_tbl[100]")" ) );

    // Every generated child expression must compile and resolve.
    ASSERT_EQ( 0, luaL_dostring( state,
        "return dbg_tbl[\"quo\\\"te\"]" ) );
    lua_settop( state, 0 );
    const auto child = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_tbl[\"line\\nbreak\"]\n0" );
    EXPECT_NE( std::string::npos,
        child.find( R"("type":"number","value":3,"entries":[])" ) );
    }

TEST_F( lua_debugger_test, browses_inherited_fields_and_cycles )
    {
    ASSERT_EQ( 0, luaL_dostring( state, R"(
        dbg_proto = { inherited = 9, shadowed = 'proto' }
        dbg_obj = setmetatable(
            { own = 1, shadowed = 'own' }, { __index = dbg_proto } )
        dbg_cycle = { x = 1 }
        setmetatable( dbg_cycle, { __index = dbg_cycle } )
        dbg_cycle.self = dbg_cycle
    )" ) );
    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_obj\n0" );
    EXPECT_NE( std::string::npos, response.find( R"("name":"inherited")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("expression":"dbg_obj[\"inherited\"]","type":"number","value":9)" ) );
    // The nearest (own) field wins over the inherited one.
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"shadowed","expression":"dbg_obj[\"shadowed\"]","type":"string","value":"own")" ) );
    EXPECT_EQ( std::string::npos, response.find( R"("value":"proto")" ) );

    // A cyclic __index chain and self references stay lazy and bounded.
    const auto cyclic = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_cycle\n0" );
    EXPECT_NE( std::string::npos, cyclic.find( R"("ok":true)" ) );
    EXPECT_NE( std::string::npos, cyclic.find( R"("name":"x")" ) );
    EXPECT_NE( std::string::npos, cyclic.find(
        R"("name":"self","expression":"dbg_cycle[\"self\"]","type":"table","value":"<table>","expandable":true)" ) );
    }

TEST_F( lua_debugger_test, browse_reports_object_identity_for_cycles )
    {
    ASSERT_EQ( 0, luaL_dostring( state,
        "dbg_identity = {}; dbg_identity.self = dbg_identity; "
        "dbg_identity.child = { back = dbg_identity }" ) );
    const auto root = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_identity\n0" );
    const auto self = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_identity.self\n0" );
    const auto child = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_identity.child\n0" );
    auto identity = []( const std::string& response )
        {
        const std::string marker = R"("object_id":")";
        const auto start = response.find( marker );
        if ( start == std::string::npos ) return std::string();
        const auto value = start + marker.size();
        return response.substr( value, response.find( '"', value ) - value );
        };
    ASSERT_FALSE( identity( root ).empty() );
    EXPECT_EQ( identity( root ), identity( self ) );
    EXPECT_NE( identity( root ), identity( child ) );
    // The back reference in the child page carries the root identity.
    EXPECT_NE( std::string::npos, child.find(
        R"("writable":false,"object_id":")" + identity( root ) + "\"" ) );
    }

TEST_F( lua_debugger_test, browse_pages_long_tables )
    {
    ASSERT_EQ( 0, luaL_dostring( state,
        "dbg_big = {}\n"
        "for i = 1, 300 do dbg_big['k' .. i] = i end\n" ) );
    const auto first = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_big\n0" );
    EXPECT_EQ( 128u, occurrences( first, R"("name":")" ) );
    EXPECT_NE( std::string::npos, first.find( R"("next_offset":128)" ) );
    EXPECT_NE( std::string::npos, first.find( R"("truncated":false)" ) );
    const auto second = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_big\n128" );
    EXPECT_EQ( 128u, occurrences( second, R"("name":")" ) );
    EXPECT_NE( std::string::npos, second.find( R"("next_offset":256)" ) );
    const auto third = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_big\n256" );
    EXPECT_EQ( 44u, occurrences( third, R"("name":")" ) );
    EXPECT_NE( std::string::npos, third.find( R"("next_offset":null)" ) );
    // The same page boundaries are deterministic.
    EXPECT_EQ( first, request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_big\n0" ) );
    }

TEST_F( lua_debugger_test, browse_pages_fit_wire_length )
    {
    // Both ordinary text and JSON-escaped values must fit the wire frame.
    for ( const auto* value : { "'v'", "string.char(1)" } )
        {
        const std::string setup =
            "dbg_wire = {}\nfor i = 1, 128 do "
            "dbg_wire[string.rep('k', 200) .. i] = string.rep(" +
            std::string( value ) + ", 256) end";
        ASSERT_EQ( 0, luaL_dostring( state, setup.c_str() ) );
        std::size_t offset = 0;
        std::string all_pages;
        do
            {
            std::string payload( 1, lua_debugger::CMD_BROWSE_VARIABLES );
            payload += session_id + "\ndbg_wire\n" + std::to_string( offset );
            // Enough storage to detect an oversized response without
            // overflowing the test buffer when the regression returns.
            std::vector<unsigned char> output( 512 * 1024 );
            const auto size = lua_debugger::process_service(
                static_cast<long>( payload.size() ),
                reinterpret_cast<unsigned char*>( payload.data() ),
                output.data() );
            ASSERT_GT( size, 0 );
            ASSERT_LE( size, 65535 ); // Includes the terminating NUL.
            ASSERT_EQ( 0, output[ size - 1 ] );
            const std::string page(
                reinterpret_cast<const char*>( output.data() ), size - 1 );
            ASSERT_NE( std::string::npos, page.find( R"("ok":true)" ) );
            const auto count = occurrences( page, R"("name":")" );
            ASSERT_GT( count, 0u );
            ASSERT_LT( count, 128u ); // Split by bytes before entry count.
            offset += count;
            ASSERT_LE( offset, 128u );
            const std::string next = offset == 128 ? "null" :
                std::to_string( offset );
            EXPECT_NE( std::string::npos,
                page.find( "\"next_offset\":" + next ) );
            all_pages += page;
            }
        while ( offset < 128 );
        for ( int i = 1; i <= 128; ++i )
            {
            const std::string name = "\"name\":\"" +
                std::string( 200, 'k' ) + std::to_string( i ) + "\"";
            EXPECT_EQ( 1u, occurrences( all_pages, name ) );
            }
        }
    }

TEST_F( lua_debugger_test, browse_bounds_long_values )
    {
    std::string long_literal( 1000, 'x' );
    ASSERT_EQ( 0, luaL_dostring( state,
        ( "dbg_long = '" + long_literal + "'" ).c_str() ) );
    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_long\n0" );
    EXPECT_LT( response.size(), 450u );
    EXPECT_NE( std::string::npos, response.find( "..." ) );
    }

TEST_F( lua_debugger_test, browse_rejects_invalid_requests )
    {
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_BROWSE_VARIABLES, "1 +\n0" ).find(
            R"("ok":false)" ) );
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_BROWSE_VARIABLES, "_G\nabc" ).find(
            R"("error":"Invalid browse offset")" ) );
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_BROWSE_VARIABLES, "_G" ).find(
            R"("error":"Missing browse offset")" ) );
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_BROWSE_VARIABLES, "\n0" ).find(
            R"("error":"Missing expression")" ) );
    EXPECT_NE( std::string::npos, raw_request(
        lua_debugger::CMD_BROWSE_VARIABLES, "unknown\n_G\n0" ).find(
            "Invalid or expired session" ) );
    }

TEST_F( lua_debugger_test, browses_userdata_properties_and_peer )
    {
    ASSERT_EQ( 0, luaL_dostring( state, R"(
        dbg_ud = newproxy( true )
        debug.setfenv( dbg_ud, { peer_scalar = 7, note = 'peer' } )
        local mt = getmetatable( dbg_ud )
        dbg_getter_calls = 0
        mt.speed_get = function( self ) return 12.5 end
        mt[ '.get' ] = {
            speed = function( self, key )
                dbg_getter_calls = dbg_getter_calls + 1
                return mt.speed_get( self )
            end,
            broken = function() error( 'getter failure', 0 ) end,
        }
        dbg_written = nil
        mt[ '.set' ] = {
            speed = function( self, value )
                dbg_written = math.floor( value + 0.5 )
            end,
        }
        mt[ '.geti' ] = function() return 0 end
        mt.__index = function( u, k )
            local peer = debug.getfenv( u )
            if type( peer ) == 'table' and peer[ k ] ~= nil then
                return peer[ k ]
            end
            local getters = getmetatable( u )[ '.get' ]
            if getters and getters[ k ] then return getters[ k ]( u, k ) end
            return getmetatable( u )[ k ]
        end
        mt.__newindex = function( u, k, v )
            local setters = getmetatable( u )[ '.set' ]
            if setters and setters[ k ] then setters[ k ]( u, v ) return end
            debug.getfenv( u )[ k ] = v
        end
    )" ) );

    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_ud\n0" );
    EXPECT_NE( std::string::npos, response.find( R"("ok":true)" ) );
    EXPECT_NE( std::string::npos,
        response.find( R"("type":"userdata")" ) );
    // Peer fields are real fields: listed and writable when scalar.
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"peer_scalar","expression":"dbg_ud[\"peer_scalar\"]","type":"number","value":7,"expandable":false,"writable":true)" ) );
    // .get property is readable; .set makes it writable.
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"speed","expression":"dbg_ud[\"speed\"]","type":"number","value":12.5,"expandable":false,"writable":true)" ) );
    // Getter errors surface in the entry instead of failing the request.
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"broken","expression":"dbg_ud[\"broken\"]","type":"error","value":"getter failure")" ) );
    // Method fields are visible but never invoked and read-only.
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"speed_get","expression":"dbg_ud[\"speed_get\"]","type":"function","value":"<function>","expandable":false,"writable":false)" ) );
    // Internal bookkeeping keys are hidden.
    EXPECT_EQ( std::string::npos, response.find( R"("name":".get")" ) );
    EXPECT_EQ( std::string::npos, response.find( R"("name":".geti")" ) );
    EXPECT_EQ( std::string::npos, response.find( R"("__newindex")" ) );

    // Only the .get getters run during browsing; the failing getter
    // aborts before incrementing the counter.
    ASSERT_EQ( 0, luaL_dostring( state, "return dbg_getter_calls" ) );
    lua_settop( state, 0 );
    const auto calls = request( lua_debugger::CMD_EVALUATE,
        "dbg_getter_calls" );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":1})", calls );
    }

TEST_F( lua_debugger_test, sets_scalar_variables )
    {
    ASSERT_EQ( 0, luaL_dostring( state,
        "dbg_num = 1\n"
        "dbg_flag = false\n"
        "dbg_text = 'old'\n"
        "dbg_tbl = { x = 1, list = { 10, 20 } }\n" ) );

    EXPECT_EQ( R"({"ok":true,"expression":"dbg_num","type":"number","value":7.5})",
        request( lua_debugger::CMD_SET_VARIABLE, "dbg_num\nnumber\n7.5" ) );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":7.5})",
        request( lua_debugger::CMD_EVALUATE, "dbg_num" ) );

    EXPECT_EQ( R"({"ok":true,"expression":"dbg_flag","type":"boolean","value":true})",
        request( lua_debugger::CMD_SET_VARIABLE, "dbg_flag\nboolean\ntrue" ) );

    const auto changed = request( lua_debugger::CMD_SET_VARIABLE,
        "dbg_text\nstring\na\"b\nc" );
    EXPECT_NE( std::string::npos, changed.find( R"("ok":true)" ) );
    EXPECT_EQ( R"({"ok":true,"type":"string","value":"a\"b\nc"})",
        request( lua_debugger::CMD_EVALUATE, "dbg_text" ) );

    // Right-hand side text is data, never Lua code.
    EXPECT_NE( std::string::npos, request( lua_debugger::CMD_SET_VARIABLE,
        "dbg_text\nstring\nos.execute('x')" ).find( R"("ok":true)" ) );
    EXPECT_EQ( R"J({"ok":true,"type":"string","value":"os.execute('x')"})J",
        request( lua_debugger::CMD_EVALUATE, "dbg_text" ) );

    EXPECT_NE( std::string::npos, request( lua_debugger::CMD_SET_VARIABLE,
        "dbg_tbl.list[2]\nnumber\n99" ).find( R"("value":99)" ) );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":99})",
        request( lua_debugger::CMD_EVALUATE, "dbg_tbl.list[2]" ) );

    // nil removes an existing table field.
    EXPECT_NE( std::string::npos, request( lua_debugger::CMD_SET_VARIABLE,
        "dbg_tbl.x\nnil\n" ).find(
            R"("ok":true,"expression":"dbg_tbl.x","type":"nil","value":null)" ) );
    EXPECT_EQ( R"({"ok":true,"type":"nil","value":null})",
        request( lua_debugger::CMD_EVALUATE, "dbg_tbl.x" ) );
    }

TEST_F( lua_debugger_test, set_variable_rejects_unsafe_requests )
    {
    ASSERT_EQ( 0, luaL_dostring( state,
        "dbg_num = 1\n"
        "dbg_tbl = { x = 1 }\n"
        "dbg_injected = false\n" ) );

    const std::string cases[] = {
        "dbg_num\nnumber\nabc",
        "dbg_num\nnumber\n1e999",
        "dbg_num\nnumber\n0x10",
        "dbg_num\nnumber\n",
        "dbg_num\nboolean\nTrue",
        "dbg_num\nint\n1",
        "dbg_num\nnil\n1",
        "dbg_tbl\nnumber\n1",            // non-scalar target
        "missing_var\nnumber\n1",        // missing target
        "dbg_tbl.missing\nnumber\n1",    // stale field
        "dbg_num.extra\nnumber\n1",      // number is not a container
        "dbg_num = error('pwn')\nnumber\n1",
        "dbg_num; dbg_injected = true\nnumber\n1",
        "dbg_num()\nnumber\n1",
        "dbg_tbl['x']()\nnumber\n1",
        "1 +\nnumber\n1",
        };
    for ( const auto& body : cases )
        {
        const auto response = request(
            lua_debugger::CMD_SET_VARIABLE, body );
        EXPECT_NE( std::string::npos, response.find( R"("ok":false)" ) )
            << body;
        }
    EXPECT_EQ( R"({"ok":true,"type":"number","value":1})",
        request( lua_debugger::CMD_EVALUATE, "dbg_num" ) );
    EXPECT_EQ( R"({"ok":true,"type":"boolean","value":false})",
        request( lua_debugger::CMD_EVALUATE, "dbg_injected" ) );
    }

TEST_F( lua_debugger_test, set_variable_on_userdata )
    {
    ASSERT_EQ( 0, luaL_dostring( state, R"(
        dbg_ud = newproxy( true )
        debug.setfenv( dbg_ud, { peer_scalar = 7 } )
        local mt = getmetatable( dbg_ud )
        dbg_written = nil
        mt[ '.get' ] = {
            speed = function() return dbg_written or 12.5 end,
            ro = function() return 1 end,
        }
        mt[ '.set' ] = {
            -- Coerces on write: the reread value must reflect it.
            speed = function( self, value )
                dbg_written = math.floor( value + 0.5 )
            end,
        }
        mt.__index = function( u, k )
            local peer = debug.getfenv( u )
            if type( peer ) == 'table' and peer[ k ] ~= nil then
                return peer[ k ]
            end
            local getters = getmetatable( u )[ '.get' ]
            if getters and getters[ k ] then return getters[ k ]( u, k ) end
            return getmetatable( u )[ k ]
        end
        mt.__newindex = function( u, k, v )
            local setters = getmetatable( u )[ '.set' ]
            if setters and setters[ k ] then setters[ k ]( u, v ) return end
            debug.getfenv( u )[ k ] = v
        end
    )" ) );

    // .set property write calls the setter; the reread shows coercion.
    EXPECT_EQ( R"({"ok":true,"expression":"dbg_ud.speed","type":"number","value":13})",
        request( lua_debugger::CMD_SET_VARIABLE,
            "dbg_ud.speed\nnumber\n12.7" ) );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":13})",
        request( lua_debugger::CMD_EVALUATE, "dbg_written" ) );

    // Existing peer scalar is writable.
    EXPECT_NE( std::string::npos, request( lua_debugger::CMD_SET_VARIABLE,
        "dbg_ud.peer_scalar\nnumber\n9" ).find( R"("value":9)" ) );

    // Property without a .set function is read-only.
    EXPECT_NE( std::string::npos, request( lua_debugger::CMD_SET_VARIABLE,
        "dbg_ud.ro\nnumber\n5" ).find( R"("error":"Property is read-only")" ) );
    // nil is rejected for bound properties.
    EXPECT_NE( std::string::npos, request( lua_debugger::CMD_SET_VARIABLE,
        "dbg_ud.speed\nnil\n" ).find( R"("error":"Property is read-only")" ) );
    // New fields may not be silently created through userdata.
    EXPECT_NE( std::string::npos, request( lua_debugger::CMD_SET_VARIABLE,
        "dbg_ud.brand_new\nstring\nx" ).find( R"("ok":false)" ) );
    }

TEST_F( lua_debugger_test, set_variable_rejects_globals_env_userdata )
    {
    // A userdata whose environment is the plain global table has no
    // dedicated peer: a same-named global must not make a .get property
    // writable, and the write must not touch the global.
    ASSERT_EQ( 0, luaL_dostring( state, R"(
        ro = 'global-value'
        dbg_gud = newproxy( true )
        local mt = getmetatable( dbg_gud )
        mt[ '.get' ] = { ro = function() return 1 end }
        mt.__index = function( u, k )
            local getters = getmetatable( u )[ '.get' ]
            if getters and getters[ k ] then return getters[ k ]( u, k ) end
            return getmetatable( u )[ k ]
        end
        mt.__newindex = function( u, k, v )
            debug.getfenv( u )[ k ] = v
        end
    )" ) );

    const auto browse = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_gud\n0" );
    EXPECT_NE( std::string::npos, browse.find(
        R"("name":"ro","expression":"dbg_gud[\"ro\"]","type":"number","value":1,"expandable":false,"writable":false)" ) );

    const auto written = request( lua_debugger::CMD_SET_VARIABLE,
        "dbg_gud.ro\nnumber\n5" );
    EXPECT_NE( std::string::npos, written.find( R"("ok":false)" ) );
    EXPECT_EQ( R"({"ok":true,"type":"string","value":"global-value"})",
        request( lua_debugger::CMD_EVALUATE, "ro" ) );
    }

TEST_F( lua_debugger_test, browse_marks_lossy_values )
    {
    ASSERT_EQ( 0, luaL_dostring( state, R"(
        dbg_lossy = {
          binary = '\128raw',   -- invalid UTF-8 byte
          literal = '\\128',    -- literal backslash + digits
        }
    )" ) );

    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_lossy\n0" );
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"binary","expression":"dbg_lossy[\"binary\"]","type":"string","value":"\\128raw","expandable":false,"writable":true,"value_lossy":true)" ) );
    // A string that merely LOOKS like an escape is not lossy — only
    // the binary entry carries the flag.
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"literal","expression":"dbg_lossy[\"literal\"]","type":"string","value":"\\128","expandable":false,"writable":true})" ) );
    EXPECT_EQ( 1u, occurrences( response, R"("value_lossy":true)" ) );

    // The set reread carries the same flag for raw stored bytes.
    const auto written = request( lua_debugger::CMD_SET_VARIABLE,
        "dbg_lossy.binary\nstring\n\x80x" );
    EXPECT_NE( std::string::npos, written.find(
        R"("value":"\\128x","value_lossy":true)" ) );
    }

TEST_F( lua_debugger_test, browse_and_set_preserve_lua_stack )
    {
    ASSERT_EQ( 0, luaL_dostring( state,
        "dbg_num = 1\ndbg_tbl = { a = 1 }\n" ) );
    const int top = lua_gettop( state );
    request( lua_debugger::CMD_BROWSE_VARIABLES, "dbg_tbl\n0" );
    EXPECT_EQ( top, lua_gettop( state ) );
    request( lua_debugger::CMD_BROWSE_VARIABLES, "1 +\n0" );
    EXPECT_EQ( top, lua_gettop( state ) );
    request( lua_debugger::CMD_SET_VARIABLE, "dbg_num\nnumber\n5" );
    EXPECT_EQ( top, lua_gettop( state ) );
    request( lua_debugger::CMD_SET_VARIABLE, "dbg_tbl\nnumber\n5" );
    EXPECT_EQ( top, lua_gettop( state ) );
    request( lua_debugger::CMD_SET_VARIABLE, "dbg_num()\nnumber\n5" );
    EXPECT_EQ( top, lua_gettop( state ) );
    }

TEST_F( lua_debugger_test, nonassignable_roots_browse_read_only )
    {
    ASSERT_EQ( 0, luaL_dostring( state,
        "dbg_factory = function() return { inner = 3 } end\n" ) );
    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_factory()\n0" );
    EXPECT_NE( std::string::npos, response.find(
        R"("expression":"(dbg_factory())[\"inner\"]")" ) );
    EXPECT_NE( std::string::npos, response.find( R"("writable":false)" ) );
    // Descendant writes are rejected: the generated path is not assignable.
    EXPECT_NE( std::string::npos, request( lua_debugger::CMD_SET_VARIABLE,
        "(dbg_factory())[\"inner\"]\nnumber\n5" ).find(
            R"("error":"Invalid assignment target")" ) );
    }

TEST_F( lua_debugger_test, reports_protocol_errors )
    {
    std::array<unsigned char, 128> input{};
    std::array<unsigned char, 1024> output{};
    lua_debugger::process_service( 0, input.data(), output.data() );
    EXPECT_STREQ( R"({"ok":false,"error":"Missing command"})",
        reinterpret_cast<const char*>( output.data() ) );

    input[ 0 ] = lua_debugger::CMD_KEEP_ALIVE;
    const std::string invalid = "unknown\n";
    std::memcpy( input.data() + 1, invalid.data(), invalid.size() );
    lua_debugger::process_service( static_cast<long>( invalid.size() + 1 ),
        input.data(), output.data() );
    EXPECT_STREQ(
        R"({"ok":false,"error":"Invalid or expired session"})",
        reinterpret_cast<const char*>( output.data() ) );

    input[ 0 ] = 255;
    const std::string valid = session_id + "\n";
    std::memcpy( input.data() + 1, valid.data(), valid.size() );
    lua_debugger::process_service( static_cast<long>( valid.size() + 1 ),
        input.data(), output.data() );
    EXPECT_STREQ( R"({"ok":false,"error":"Unknown command"})",
        reinterpret_cast<const char*>( output.data() ) );
    }

TEST_F( lua_debugger_test, set_variable_writes_globals_named_value )
    {
    // A global literally named `value` must not be shadowed by an
    // internal local in the generated write chunk.
    ASSERT_EQ( 0, luaL_dostring( state,
        "value = 1\n"
        "holder = { value = { x = 2 } }\n" ) );

    EXPECT_EQ( R"({"ok":true,"expression":"value","type":"number","value":42})",
        request( lua_debugger::CMD_SET_VARIABLE, "value\nnumber\n42" ) );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":42})",
        request( lua_debugger::CMD_EVALUATE, "value" ) );

    EXPECT_EQ( R"({"ok":true,"expression":"holder.value.x","type":"number","value":7})",
        request( lua_debugger::CMD_SET_VARIABLE,
            "holder.value.x\nnumber\n7" ) );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":7})",
        request( lua_debugger::CMD_EVALUATE, "holder.value.x" ) );

    // nil removes a real global instead of faking success.
    EXPECT_NE( std::string::npos, request( lua_debugger::CMD_SET_VARIABLE,
        "value\nnil\n" ).find( R"("type":"nil","value":null)" ) );
    EXPECT_EQ( R"({"ok":true,"type":"nil","value":null})",
        request( lua_debugger::CMD_EVALUATE, "value" ) );
    }

TEST_F( lua_debugger_test, browse_and_set_mark_truncated_strings )
    {
    std::string long_literal( 300, 'x' );
    ASSERT_EQ( 0, luaL_dostring( state,
        ( "dbg_tbl = { s = '" + long_literal + "' }" ).c_str() ) );

    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_tbl\n0" );
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"s","expression":"dbg_tbl[\"s\"]","type":"string")" ) );
    EXPECT_NE( std::string::npos,
        response.find( R"("value_truncated":true)" ) );

    // The reread of a long written string carries the same metadata.
    const std::string payload = "dbg_tbl.s\nstring\n" +
        std::string( 300, 'y' );
    const auto written = request( lua_debugger::CMD_SET_VARIABLE,
        payload );
    EXPECT_NE( std::string::npos, written.find( R"("ok":true)" ) );
    EXPECT_NE( std::string::npos,
        written.find( R"("value_truncated":true)" ) );
    EXPECT_LT( written.size(), 700u );
    }

TEST_F( lua_debugger_test, browse_bounds_huge_keys_and_advances )
    {
    ASSERT_EQ( 0, luaL_dostring( state,
        "dbg_bigkey = {}\n"
        "dbg_bigkey[ string.rep( 'k', 100000 ) ] = 1\n"
        "dbg_bigkey.short = 2\n" ) );
    const int top = lua_gettop( state );

    const auto first = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_bigkey\n0" );
    EXPECT_NE( std::string::npos, first.find( R"("ok":true)" ) );
    // The name is bounded and the entry cannot be addressed or written.
    EXPECT_NE( std::string::npos, first.find( R"("expression":"")" ) );
    EXPECT_NE( std::string::npos, first.find( R"("writable":false)" ) );
    EXPECT_NE( std::string::npos, first.find( R"("truncated":true)" ) );
    EXPECT_LT( first.size(), 8000u );
    EXPECT_EQ( top, lua_gettop( state ) );
    // The bounded page also contains the regular sibling entry.
    EXPECT_NE( std::string::npos, first.find(
        R"("name":"short","expression":"dbg_bigkey[\"short\"]")" ) );
    EXPECT_NE( std::string::npos, first.find( R"("next_offset":null)" ) );

    // Explicit offsets still page deterministically past the
    // unaddressable entry.
    const auto second = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_bigkey\n1" );
    EXPECT_NE( std::string::npos, second.find(
        R"("name":"short","expression":"dbg_bigkey[\"short\"]")" ) );
    EXPECT_NE( std::string::npos, second.find( R"("next_offset":null)" ) );
    }

TEST_F( lua_debugger_test, browse_marks_scan_limit_and_rejects_bad_offsets )
    {
    ASSERT_EQ( 0, luaL_dostring( state,
        "dbg_many = {}\n"
        "for i = 1, 5000 do dbg_many[ 'f' .. i ] = i end\n" ) );
    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_many\n0" );
    EXPECT_NE( std::string::npos, response.find( R"("truncated":true)" ) );
    EXPECT_NE( std::string::npos,
        request( lua_debugger::CMD_BROWSE_VARIABLES, "_G\n-1" ).find(
            R"("error":"Invalid browse offset")" ) );
    EXPECT_NE( std::string::npos, request(
        lua_debugger::CMD_BROWSE_VARIABLES,
        "_G\n99999999999999999999" ).find(
            R"("error":"Invalid browse offset")" ) );
    }

TEST_F( lua_debugger_test, browse_escapes_malformed_utf8 )
    {
    ASSERT_EQ( 0, luaL_dostring( state, R"(
        dbg_utf = {
          ['\192\175bad'] = 1,           -- overlong sequence C0 AF
          ['\128orphan'] = 2,            -- isolated continuation
          ['\237\160\128x'] = 3,         -- UTF-16 surrogate
          ['\244\144\128\128y'] = 4,     -- above U+10FFFF
          ['\245z'] = 5,                 -- invalid lead byte
          ['привет'] = 6,                -- valid Cyrillic
        }
    )" ) );

    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_utf\n0" );
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"\\192\\175bad")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"\\128orphan")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"\\237\\160\\128x")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"\\244\\144\\128\\128y")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"\\245z")" ) );
    // Valid multibyte text survives untouched.
    EXPECT_NE( std::string::npos, response.find( "привет" ) );
    // Decimal escapes in generated expressions compile to the same key.
    EXPECT_NE( std::string::npos, response.find(
        R"("expression":"dbg_utf[\"\\192\\175bad\"]")" ) );
    const auto child = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_utf[\"\\192\\175bad\"]\n0" );
    EXPECT_NE( std::string::npos,
        child.find( R"("type":"number","value":1,"entries":[])" ) );
    }

TEST_F( lua_debugger_test, browse_preserves_multibyte_at_preview_edge )
    {
    // A two-byte Cyrillic letter straddling the preview boundary is not
    // split mid-sequence.
    std::string script = "dbg_edge = '";
    script += std::string( 255, 'a' );
    script += "я";   // bytes D1 8F at positions 255-256
    script += "'";
    ASSERT_EQ( 0, luaL_dostring( state, script.c_str() ) );
    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_edge\n0" );
    EXPECT_NE( std::string::npos, response.find( R"("ok":true)" ) );
    EXPECT_NE( std::string::npos,
        response.find( R"("value_truncated":true)" ) );
    // The boundary cut keeps all 'a' bytes and appends an ellipsis;
    // the Cyrillic lead byte is dropped, not decimal-escaped.
    EXPECT_NE( std::string::npos, response.find(
        "\"value\":\"" + std::string( 255, 'a' ) + "...\"" ) );
    EXPECT_EQ( std::string::npos, response.find( R"(\\209)" ) );
    }

TEST_F( lua_debugger_test, browse_enumerates_nul_keys )
    {
    ASSERT_EQ( 0, luaL_dostring( state, R"(
        dbg_nul = {}
        dbg_nul[ 'a\0b' ] = 5
    )" ) );
    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_nul\n0" );
    EXPECT_NE( std::string::npos,
        response.find( R"("name":"a\u0000b")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("expression":"dbg_nul[\"a\\000b\"]")" ) );
    const auto child = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_nul[\"a\\000b\"]\n0" );
    EXPECT_NE( std::string::npos,
        child.find( R"("type":"number","value":5,"entries":[])" ) );
    }

TEST_F( lua_debugger_test, userdata_getter_mutation_is_safe )
    {
    ASSERT_EQ( 0, luaL_dostring( state, R"(
        dbg_mut = newproxy( true )
        local mt = getmetatable( dbg_mut )
        mt[ '.get' ] = {
            mutate = function( self )
                getmetatable( self )[ '.get' ].mutate = nil
                getmetatable( self )[ '.get' ].other = nil
                return 1
            end,
            other = function() return 2 end,
        }
        mt.__index = function( u, k )
            local getters = getmetatable( u )[ '.get' ]
            if getters and getters[ k ] then return getters[ k ]( u, k ) end
        end
    )" ) );
    const int top = lua_gettop( state );
    // A getter deleting entries from the table being enumerated must
    // not panic or corrupt the Lua stack.
    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_mut\n0" );
    EXPECT_NE( std::string::npos, response.find( R"("ok":true)" ) );
    EXPECT_NE( std::string::npos, response.find( R"("name":"mutate")" ) );
    EXPECT_EQ( top, lua_gettop( state ) );
    }

TEST_F( lua_debugger_test, browse_bounds_getter_error_text )
    {
    ASSERT_EQ( 0, luaL_dostring( state, R"(
        dbg_gerr = newproxy( true )
        local mt = getmetatable( dbg_gerr )
        mt[ '.get' ] = {
            boom = function() error( string.rep( 'e', 5000 ), 0 ) end,
        }
        mt.__index = function() return nil end
    )" ) );
    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_gerr\n0" );
    EXPECT_NE( std::string::npos, response.find( R"("name":"boom")" ) );
    EXPECT_NE( std::string::npos, response.find( R"("type":"error")" ) );
    EXPECT_LT( response.size(), 2000u );
    }

TEST_F( lua_debugger_test, browse_only_invokes_page_getters )
    {
    // With more .get properties than a page, enumeration alone must not
    // call every getter — only the ones in the requested page.
    std::string script =
        "dbg_pages = newproxy( true )\n"
        "dbg_get_calls = 0\n"
        "local mt = getmetatable( dbg_pages )\n"
        "mt[ '.get' ] = {}\n"
        "for i = 1, 200 do\n"
        "  mt[ '.get' ][ 'p' .. i ] = function()\n"
        "    dbg_get_calls = dbg_get_calls + 1\n"
        "    return i\n"
        "  end\n"
        "end\n"
        "mt.__index = function( u, k )\n"
        "  local g = getmetatable( u )[ '.get' ]\n"
        "  if g and g[ k ] then return g[ k ]( u, k ) end\n"
        "end\n";
    ASSERT_EQ( 0, luaL_dostring( state, script.c_str() ) );
    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_pages\n0" );
    EXPECT_NE( std::string::npos, response.find( R"("ok":true)" ) );
    const auto calls = request( lua_debugger::CMD_EVALUATE,
        "dbg_get_calls" );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":128})", calls );
    }

TEST_F( lua_debugger_test, browses_real_tolua_object_properties )
    {
    ASSERT_EQ( 1, tolua_PAC_dev_open( state ) );
    ASSERT_EQ( 0, luaL_dostring( state,
        "dbg_cip = cipline_tech_object("
        " 'CIPDBG', 1, 111, 'CIPDBG1', 1, 1, 1, 0, 1, 0 )" ) );

    const auto response = request( lua_debugger::CMD_BROWSE_VARIABLES,
        "dbg_cip\n0" );
    EXPECT_NE( std::string::npos, response.find( R"("ok":true)" ) );
    EXPECT_NE( std::string::npos,
        response.find( R"("type":"userdata")" ) );
    EXPECT_NE( std::string::npos, response.find(
        R"("name":"blocked","expression":"dbg_cip[\"blocked\"]","type":"number")" ) );
    EXPECT_NE( std::string::npos,
        response.find( R"("writable":true)" ) );

    // The real .set path applies the write and the reread shows it.
    EXPECT_NE( std::string::npos, request(
        lua_debugger::CMD_SET_VARIABLE,
        "dbg_cip.blocked\nnumber\n5" ).find( R"("value":5)" ) );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":5})",
        request( lua_debugger::CMD_EVALUATE, "dbg_cip.blocked" ) );

    lua_getglobal( state, "dbg_cip" );
    auto* object = static_cast<cipline_tech_object*>(
        tolua_tousertype( state, -1, nullptr ) );
    ASSERT_NE( nullptr, object );
    EXPECT_EQ( 5, object->blocked );
    lua_pop( state, 1 );
    }

TEST_F( lua_debugger_test, set_variable_preserves_raw_nul_bytes )
    {
    ASSERT_EQ( 0, luaL_dostring( state, "dbg_s = ''" ) );

    // Interior and trailing NUL bytes reach Lua as data, not as the end
    // of the raw payload.
    const std::string interior( "a\0b", 3 );
    EXPECT_NE( std::string::npos, request(
        lua_debugger::CMD_SET_VARIABLE,
        "dbg_s\nstring\n" + interior ).find( R"("ok":true)" ) );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":3})",
        request( lua_debugger::CMD_EVALUATE, "#dbg_s" ) );

    const std::string trailing( "a\0", 2 );
    EXPECT_NE( std::string::npos, request(
        lua_debugger::CMD_SET_VARIABLE,
        "dbg_s\nstring\n" + trailing ).find( R"("ok":true)" ) );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":2})",
        request( lua_debugger::CMD_EVALUATE, "#dbg_s" ) );

    const std::string multi( "x\0\0\0", 4 );
    EXPECT_NE( std::string::npos, request(
        lua_debugger::CMD_SET_VARIABLE,
        "dbg_s\nstring\n" + multi ).find( R"("ok":true)" ) );
    EXPECT_EQ( R"({"ok":true,"type":"number","value":4})",
        request( lua_debugger::CMD_EVALUATE, "#dbg_s" ) );
    }

TEST_F( lua_debugger_test, evaluate_sanitizes_malformed_lua_errors )
    {
    // Lua errors may embed bytes that are not valid UTF-8; responses
    // must stay well-formed.
    const auto response = request( lua_debugger::CMD_EVALUATE,
        R"(error( 'bad\192\175x', 0 ))" );
    EXPECT_NE( std::string::npos, response.find( R"("ok":false)" ) );
    EXPECT_NE( std::string::npos,
        response.find( R"(bad\\192\\175x)" ) );

    ASSERT_EQ( R"({"ok":true,"count":1})",
        request( lua_debugger::CMD_SET_CHART_EXPRESSIONS,
            R"(error( 'e\128x', 0 ))" ) );
    G_LUA_DEBUGGER->evaluate();
    const auto chart = request( lua_debugger::CMD_GET_CHART_DATA );
    EXPECT_NE( std::string::npos, chart.find( R"(e\\128x)" ) );
    }
