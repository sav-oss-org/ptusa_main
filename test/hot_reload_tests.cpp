#include "includes.h"
#include "tech_def.h"
#include "lua_manager.h"
#include "lua_debugger.h"
#include "g_errors.h"
#include "prj_mngr.h"
#include <filesystem>
#include <fstream>
#include <chrono>
#include <array>

namespace
{
class reload_manager : public tech_object_manager {};
tech_object_manager* test_manager = nullptr;
tech_object_manager* get_test_manager() { return test_manager; }

const char* description = R"lua(
{
 name='Tank', n=1, tech_type=1, timers=1,
 par_float={{nameLua='TIME',value=10},{nameLua='OTHER',value=20}},
 par_uint={{nameLua='COUNT',value=3}},
 rt_par_float={{nameLua='VALUE'}}, rt_par_uint={{nameLua='STATE'}},
 modes={{name='Process', states={[1]={steps={
     {name='Original', baseStep='WORK', next_step_n=-1, time_param_n=1}
 }}}}}
}
)lua";

class hot_reload_test : public ::testing::Test
    {
    protected:
        void SetUp() override
            {
            L = lua_open();
            luaL_openlibs( L );
            tolua_PAC_dev_open( L );
            G_LUA_MANAGER->set_Lua( L );
            G_ERRORS_MANAGER->clear();
            test_manager = &manager;
            hook = subhook_new( reinterpret_cast<void*>( &G_TECH_OBJECT_MNGR ),
                reinterpret_cast<void*>( &get_test_manager ), SUBHOOK_64BIT_OFFSET );
            subhook_install( hook );
            previous_path = G_PROJECT_MANAGER->path;
            directory = std::filesystem::current_path() /
                ("reload_fixture_" + std::to_string(
                    std::chrono::steady_clock::now().time_since_epoch().count()));
            std::filesystem::create_directories( directory );
            const auto directory_utf8 = directory.u8string();
            G_PROJECT_MANAGER->path.assign( directory_utf8.begin(), directory_utf8.end() ); // No trailing slash.
            const auto system_script = std::filesystem::path( __FILE__ ).parent_path().parent_path() /
                "demo_projects/T1-PLCnext-Demo/sys/sys.objects.lua";
            const auto script_utf8 = system_script.u8string();
            const std::string script_path( script_utf8.begin(), script_utf8.end() );
            ASSERT_EQ( 0, luaL_dofile( L, script_path.c_str() ) )
                << (lua_tostring( L, -1 ) ? lua_tostring( L, -1 ) : "");
            const std::string init = "function init_tech_objects_modes() return {" +
                std::string( description ) + ", (function() local obj=" + description +
                "; obj.n=42; obj.name='Other'; return obj end)()} end; init_tech_objects()";
            ASSERT_EQ( 0, luaL_dostring( L, init.c_str() ) );
            lua_getglobal( L, "OBJECT1" );
            lua_getfield( L, -1, "sys_tech_object" );
            object = static_cast<tech_object*>( tolua_tousertype( L, -1, nullptr ) );
            lua_pop( L, 2 );
            ASSERT_NE( nullptr, object );
            manager.add_tech_object( object );
            lua_getglobal( L, "OBJECT2" );
            lua_getfield( L, -1, "sys_tech_object" );
            manager.add_tech_object( static_cast<tech_object*>(
                tolua_tousertype( L, -1, nullptr ) ) );
            lua_pop( L, 2 );
            object->par_float[1] = 123.5f;
            object->par_uint[1] = 23;
            object->rt_par_float[1] = 45.5f;
            object->rt_par_uint[1] = 67;
            write_objects();
            }

        void TearDown() override
            {
            subhook_remove( hook );
            subhook_free( hook );
            test_manager = nullptr;
            G_ERRORS_MANAGER->clear();
            G_DEVICE_CMMCTR->clear_devices();
            G_LUA_MANAGER->free_Lua();
            G_PROJECT_MANAGER->path = previous_path;
            std::filesystem::remove_all( directory );
            }

        void write_objects( const std::string& change = {} )
            {
            std::ofstream file( directory / "main.objects.lua" );
            file << "local obj = " << description << "\n"
                 << "obj.modes[1].states[1].steps[1].name = 'Reloaded'\n"
                 << change << "\nfunction init_tech_objects_modes() return {obj} end\n";
            }

        int reload() { return manager.reload_object( 1 ); }
        std::string request( int command, const std::string& body )
            {
            std::array<unsigned char, 4096> input{};
            std::array<unsigned char, 65536> output{};
            input[0] = static_cast<unsigned char>( command );
            memcpy( input.data() + 1, body.data(), body.size() );
            const auto size = lua_debugger::process_service(
                static_cast<long>( body.size() + 1 ), input.data(), output.data() );
            return std::string( reinterpret_cast<char*>( output.data() ),
                size > 0 ? size - 1 : 0 );
            }

        reload_manager manager;
        lua_State* L{};
        tech_object* object{};
        subhook_t hook{};
        std::filesystem::path directory;
        std::string previous_path;
    };
}

TEST_F( hot_reload_test, repeated_reload_preserves_parameters_and_object_identity )
    {
    auto* saved_float = &object->par_float[1];
    auto* saved_uint = &object->par_uint[1];
    int before = 0, after = 0;
    params_manager::get_instance()->reserve_params_region( 0, before );
    ASSERT_EQ( 0, luaL_dostring( L,
        "saved_wrapper=OBJECT1; saved_sys=OBJECT1.sys_tech_object; "
        "saved_par=OBJECT1.par_float; saved_timers=OBJECT1.timers" ) );
    for ( int i = 0; i < 30; ++i )
        {
        ASSERT_EQ( 0, reload() ) << manager.get_reload_error();
        EXPECT_EQ( object, manager.get_tech_objects( 0 ) );
        EXPECT_EQ( saved_float, &object->par_float[1] );
        EXPECT_EQ( saved_uint, &object->par_uint[1] );
        EXPECT_EQ( 123.5f, object->par_float[1] );
        EXPECT_EQ( 23u, object->par_uint[1] );
        EXPECT_EQ( 45.5f, object->rt_par_float[1] );
        EXPECT_EQ( 67u, object->rt_par_uint[1] );
        auto* run_state = (*(*object->get_modes_manager())[1])[operation::RUN];
        EXPECT_STREQ( "Reloaded", (*run_state)[1]->get_name() );
        }
    params_manager::get_instance()->reserve_params_region( 0, after );
    EXPECT_EQ( before, after );
    ASSERT_EQ( 0, luaL_dostring( L,
        "assert(saved_wrapper==OBJECT1 and saved_sys==OBJECT1.sys_tech_object "
        "and saved_par==OBJECT1.par_float and saved_timers==OBJECT1.timers); "
        "saved_par[1]=789" ) );
    EXPECT_EQ( 789.0f, saved_float[0] ); // Original NVRAM address is still written.
    }

TEST_F( hot_reload_test, cooperate_parameter_accepts_disabled_values_and_valid_indices )
    {
    for ( const int value : { -1, 0, 1, 2 } )
        {
        SCOPED_TRACE( value );
        write_objects( "obj.cooper_param_number=" + std::to_string( value ) );
        EXPECT_EQ( 0, reload() ) << manager.get_reload_error();
        EXPECT_EQ( 123.5f, object->par_float[1] );
        }
    }

TEST_F( hot_reload_test, failed_reload_is_atomic_and_does_not_reserve_nvram )
    {
    auto* original_modes = object->get_modes_manager();
    int before = 0, after = 0;
    params_manager::get_instance()->reserve_params_region( 0, before );
    const char* changes[] = {
        "this is not Lua!",
        "return", // Reject malformed Lua before changing live operations.
        "obj.n=99",
        "obj.tech_type=112",
        "obj.timers=2",
        "obj.par_float[1],obj.par_float[2]=obj.par_float[2],obj.par_float[1]",
        "obj.par_float[3]={nameLua='NEW',value=1}",
        "obj.modes[1].name='Different operation'",
        "obj.modes[2]=obj.modes[1]",
        "obj.cooper_param_number=-2",
        "obj.cooper_param_number=3",
        "obj.cooper_param_number=1.5",
        "obj.cooper_param_number='1'",
        "obj.modes[1].states[1].steps[1].baseStep='OTHER'",
        "obj.modes[1].states[1].steps[1].baseStep=nil",
        "obj.modes[1].states[1].steps={}",
        "obj.modes[1].states[1]=nil",
        "obj.modes[1].states[1].steps[2]={name='New',baseStep='NEW'}",
        "obj.modes[1].states[1].steps[1].time_param_n=99",
        "obj.modes[1].states[1].steps[1].next_step_n=99",
        "obj.modes[1].states[9]={steps={}}",
        "obj.modes[1].states[1].steps[1].opened_devices={'UNKNOWN'}",
        "obj.modes[1].states[1].steps[1].devices_data={{pump_freq='UNKNOWN'}}",
        "obj.modes[1].states[1].steps[1].devices_data={{pump_freq=99}}",
        "obj.modes[1].states[1].steps[1].devices_data={{unknown_field={}}}",
        "obj.modes[1].states[1].steps[1].jump_if={[2]={next_step_n=1}}",
        "obj.modes[1].states[1].steps[1].unsupported=true",
        "OBJECT1.par_float[1]=0", // Module environment cannot access the PAC.
        };
    for ( const auto* change : changes )
        {
        SCOPED_TRACE( change );
        write_objects( change );
        const int top = lua_gettop( L );
        EXPECT_EQ( -3, reload() );
        EXPECT_FALSE( manager.get_reload_error().empty() );
        EXPECT_EQ( original_modes, object->get_modes_manager() );
        EXPECT_EQ( 123.5f, object->par_float[1] );
        EXPECT_EQ( top, lua_gettop( L ) );
        }
    std::filesystem::remove( directory / "main.objects.lua" );
    EXPECT_EQ( -3, reload() );
    params_manager::get_instance()->reserve_params_region( 0, after );
    EXPECT_EQ( before, after );
    write_objects();
    EXPECT_EQ( 0, reload() ) << manager.get_reload_error();
    }

TEST_F( hot_reload_test, shared_file_reloads_only_selected_serial_index )
    {
    // The second object's n deliberately differs from its serial index (2).
    const std::string second = "local obj=" + std::string(description) +
        "; obj.n=42; obj.name='Other'; return obj";
    lua_getglobal( L, "OBJECT2" );
    lua_getfield( L, -1, "sys_tech_object" );
    auto* other = static_cast<tech_object*>( tolua_tousertype( L, -1, nullptr ) );
    lua_pop( L, 2 );
    ASSERT_NE( nullptr, other );
    auto* first_modes = object->get_modes_manager();
    ASSERT_EQ( 0, object->set_mode( 1, operation::RUN ) );
    {
    std::ofstream file( directory / "main.objects.lua" );
    file << "function init_tech_objects_modes() local obj=(function() " << second
         << " end)(); obj.modes[1].states[operation.RUN].steps[1].name='Selected'; "
         << "return { {}, obj } end";
    }
    EXPECT_EQ( 0, manager.reload_object( 2 ) ) << manager.get_reload_error();
    EXPECT_EQ( first_modes, object->get_modes_manager() );
    EXPECT_EQ( operation::RUN, object->get_operation_state( 1 ) );
    EXPECT_STREQ( "Original", (*(*(*first_modes)[1])[operation::RUN])[1]->get_name() );
    EXPECT_STREQ( "Selected",
        (*(*(*other->get_modes_manager())[1])[operation::RUN])[1]->get_name() );
    }

TEST_F( hot_reload_test, shared_file_errors_are_atomic_and_globals_are_isolated )
    {
    auto* original_modes = object->get_modes_manager();
    ASSERT_EQ( 0, luaL_dostring( L,
        "saved_init=init_tech_objects_modes; reload_marker='live'" ) );
    const char* sources[] = {
        "return {}", // No initializer; never use the live global as a fallback.
        "init_tech_objects_modes=1",
        "function init_tech_objects_modes() return false end",
        "function init_tech_objects_modes() return {} end",
        "function init_tech_objects_modes() return {false} end",
        "function init_tech_objects_modes() return OBJECT1 end",
        "require('prg')",
    };
    for ( const auto* source : sources )
        {
        SCOPED_TRACE( source );
        {
        std::ofstream file( directory / "main.objects.lua" );
        file << source;
        }
        const int top = lua_gettop( L );
        EXPECT_EQ( -3, reload() );
        EXPECT_EQ( top, lua_gettop( L ) );
        EXPECT_EQ( original_modes, object->get_modes_manager() );
        }
    write_objects("reload_marker='sandbox'");
    EXPECT_EQ( 0, reload() ) << manager.get_reload_error();
    ASSERT_EQ( 0, luaL_dostring( L,
        "assert(init_tech_objects_modes==saved_init and reload_marker=='live')" ) );
    }

TEST_F( hot_reload_test, source_larger_than_one_mib_reloads )
    {
    // Put the initializer beyond the former read limit to check full-file loading.
    write_objects( "--" + std::string( 1024 * 1024, 'x' ) );
    ASSERT_GT( std::filesystem::file_size( directory / "main.objects.lua" ),
        1024u * 1024u );
    const int top = lua_gettop( L );
    ASSERT_EQ( 0, reload() ) << manager.get_reload_error();
    EXPECT_EQ( top, lua_gettop( L ) );
    EXPECT_STREQ( "Reloaded",
        (*(*(*object->get_modes_manager())[1])[operation::RUN])[1]->get_name() );
    EXPECT_EQ( 123.5f, object->par_float[1] );
    }

TEST_F( hot_reload_test, module_and_initializer_can_exceed_former_instruction_limit )
    {
    const char* loops[] = {
        "local count=0; for i=1,1000000 do count=count+1 end; "
        "if count==1000000 then obj.modes[1].states[1].steps[1].name='Long module' end",
        "local initial=init_tech_objects_modes; function init_tech_objects_modes() "
        "local count=0; for i=1,1000000 do count=count+1 end; "
        "local objects=initial(); if count==1000000 then "
        "objects[1].modes[1].states[1].steps[1].name='Long initializer' end; "
        "return objects end",
    };
    const char* names[] = { "Long module", "Long initializer" };
    for ( int i = 0; i < 2; ++i )
        {
        SCOPED_TRACE( names[i] );
        write_objects();
        {
        std::ofstream file( directory / "main.objects.lua", std::ios::app );
        file << loops[i];
        }
        const int top = lua_gettop( L );
        ASSERT_EQ( 0, reload() ) << manager.get_reload_error();
        EXPECT_EQ( top, lua_gettop( L ) );
        EXPECT_STREQ( names[i],
            (*(*(*object->get_modes_manager())[1])[operation::RUN])[1]->get_name() );
        }
    }

TEST_F( hot_reload_test, busy_objects_are_rejected )
    {
    for ( const auto state : { operation::RUN, operation::PAUSE, operation::STOP } )
        {
        ASSERT_EQ( 0, object->set_mode( 1, state ) );
        EXPECT_EQ( -2, reload() );
        }
    }

TEST_F( hot_reload_test, reload_from_operation_callback_is_rejected )
    {
    auto* original_modes = object->get_modes_manager();
    ASSERT_EQ( 0, luaL_dostring( L,
        "function OBJECT1:check_on_mode() "
        "nested_result=SYSTEM:set_cmd('CMD',0,1030001); return 0 end" ) );
    ASSERT_EQ( 0, object->set_mode( 1, operation::RUN ) );
    lua_getglobal( L, "nested_result" );
    EXPECT_EQ( -2, lua_tointeger( L, -1 ) );
    lua_pop( L, 1 );
    EXPECT_EQ( original_modes, object->get_modes_manager() );
    EXPECT_EQ( operation::RUN, object->get_operation_state( 1 ) );
    ASSERT_EQ( 0, object->set_mode( 1, operation::IDLE ) );
    EXPECT_EQ( 0, reload() ) << manager.get_reload_error();
    }

TEST_F( hot_reload_test, retained_lua_handles_reject_reload_until_released )
    {
    for ( const auto* expression : {
        "OBJECT1:get_modes_manager()", "OBJECT1:get_modes_manager()[1]",
        "OBJECT1:get_modes_manager()[1][1]", "OBJECT1:get_modes_manager()[1][1][1]",
        "OBJECT1:get_modes_manager()[1][1][1][step.A_ON]" } )
        {
        SCOPED_TRACE( expression );
        const auto assignment = std::string("held = ") + expression;
        ASSERT_EQ( 0, luaL_dostring( L, assignment.c_str() ) );
        EXPECT_EQ( -4, reload() );
        ASSERT_EQ( 0, luaL_dostring( L, "held = nil" ) );
        EXPECT_EQ( 0, reload() ) << manager.get_reload_error();
        }
    }

TEST_F( hot_reload_test, missing_object_and_unavailable_builder )
    {
    EXPECT_EQ( -1, manager.reload_object( 999 ) );
    const char* scripts[] = {
        "prepare_tech_object_reload=nil",
        "function prepare_tech_object_reload() error('failed') end",
        "function prepare_tech_object_reload() return io.stdout end",
        "function prepare_tech_object_reload() return true end", // Empty graph.
        };
    for ( const auto* script : scripts )
        {
        ASSERT_EQ( 0, luaL_dostring( L, script ) );
        const int top = lua_gettop( L );
        EXPECT_EQ( -3, reload() );
        EXPECT_EQ( top, lua_gettop( L ) );
        }
    }

TEST_F( hot_reload_test, debugger_lists_objects_reloads_and_reports_failure )
    {
    auto created = request( lua_debugger::CMD_CREATE_SESSION, "" );
    const std::string marker = "\"session_id\":\"";
    auto start = created.find(marker) + marker.size();
    auto session = created.substr(start, created.find('"', start) - start) + "\n";
    const auto listed = request( lua_debugger::CMD_GET_RELOAD_OBJECTS, session );
    EXPECT_NE( std::string::npos, listed.find("\"id\":1") );
    EXPECT_NE( std::string::npos, listed.find("\"lua_name\":\"OBJECT1\"") );
    EXPECT_NE( std::string::npos, listed.find("\"idle\":true") );
    const auto success = request( lua_debugger::CMD_EXEC_CONTROLLER_COMMAND, session+"1030001" );
    EXPECT_NE( std::string::npos, success.find("\"ok\":true") ) << success;
    write_objects("obj.n=2");
    const auto failure = request( lua_debugger::CMD_EXEC_CONTROLLER_COMMAND, session+"1030001" );
    EXPECT_NE( std::string::npos, failure.find("\"result\":-3") ) << failure;
    EXPECT_NE( std::string::npos, failure.find("Changed object.n") ) << failure;
    }
