#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <map>
#include <mutex>
#include <string>
#include <vector>

extern "C"
    {
#include "lua.h"
#include "lauxlib.h"
    }

/// Cycle-synchronous debugger for the Lua control program.
///
/// The debugger is exposed as service 2 of the regular TCP protocol (port
/// 10000). A request payload starts with one byte from COMMAND. Except for
/// CMD_CREATE_SESSION, the remaining payload starts with a hexadecimal session
/// id and a newline, followed by UTF-8 command text where applicable. Text
/// responses are zero-terminated JSON.
class lua_debugger
    {
    public:
        enum CONSTANTS
            {
            C_SERVICE_N = 2,
            MAX_SESSIONS = 16,
            MAX_EXPRESSIONS = 16,
            MAX_SAMPLES_PER_EXPRESSION = 64,
            MAX_MESSAGES = 64,
            MAX_MESSAGES_PER_RESPONSE = 32,
            SESSION_TIMEOUT_MS = 10'000,
            };

        enum COMMAND : unsigned char
            {
            CMD_CREATE_SESSION = 1,
            CMD_EVALUATE,
            /// Newline-separated Lua expressions. Replaces the session list.
            CMD_SET_CHART_EXPRESSIONS,
            CMD_GET_CHART_DATA,
            /// Clears samples, but keeps the configured expressions.
            CMD_CLEAR_CHART_DATA,
            CMD_CLOSE_SESSION,
            CMD_KEEP_ALIVE,
            /// Returns messages accumulated since the previous request.
            CMD_GET_MESSAGES,
            CMD_POLL,
            /// Decimal PAC_info::COMMANDS id in the request body.
            CMD_EXEC_CONTROLLER_COMMAND,
            /// Returns object ids/names and idle state for individual reload.
            CMD_GET_RELOAD_OBJECTS,
            /// Returns one page of child variables for a root expression.
            CMD_BROWSE_VARIABLES,
            /// Assigns a scalar value to an existing scalar variable.
            CMD_SET_VARIABLE,
            /// Executes a Lua chunk; returns its first result (nil if absent).
            CMD_EXECUTE,
            };

        static lua_debugger* get_instance();

        /// Called once per PAC cycle, after the control Lua scripts ran.
        void evaluate();

        /// TCP service callback. See COMMAND for the wire format.
        static long process_service( long len, unsigned char* data,
            unsigned char* outdata );

        /// Releases all sessions while the supplied Lua state is valid.
        void reset( lua_State* state = nullptr );

        /// Publishes a message to debugger sessions that are currently open.
        void publish_message( const char* source, int priority,
            const char* text );

#ifdef PTUSA_TEST
        std::size_t sessions_count() const;
        std::size_t expressions_count( const std::string& session_id ) const;
        void expire_sessions_for_test( std::uint32_t elapsed_ms );
#endif

    private:
        struct value
            {
            bool ok = false;
            std::string type;
            std::string json;

            bool operator==( const value& rhs ) const;
            };

        struct sample
            {
            std::uint32_t time_ms = 0;
            value data;
            };

        struct expression
            {
            std::string source;
            int lua_ref = LUA_NOREF;
            std::deque<sample> samples;
            };

        struct session
            {
            std::uint32_t last_access_ms = 0;
            std::uint64_t next_message_id = 0;
            std::vector<expression> expressions;
            };

        struct message
            {
            std::uint64_t id = 0;
            std::uint32_t time_ms = 0;
            std::string source;
            int priority = 0;
            std::string text;
            };

        lua_debugger() = default;

        value evaluate_source( const std::string& source ) const;
        value execute_source( const std::string& source ) const;
        value evaluate_ref( int lua_ref ) const;
        value value_from_stack( lua_State* state, int index,
            std::size_t max_string_length ) const;
        std::string create_session();
        std::string set_expressions( session& target,
            const std::string& request );
        std::string browse_variables( const std::string& request ) const;
        std::string set_variable( const std::string& request ) const;
        std::string chart_data( const session& target ) const;
        std::string message_data( session& target, std::size_t budget = 60000 );
        void clear_samples( session& target );
        void release_expressions( session& target );
        void expire_sessions();

        static std::string json_quote( const std::string& source );
        static long write_response( const std::string& response,
            unsigned char* outdata );

        lua_State* state_ = nullptr;
        std::map<std::string, session> sessions_;
        std::uint64_t next_session_id_ = 1;
        std::deque<message> messages_;
        std::uint64_t next_message_id_ = 1;
        std::mutex messages_mutex_;
        std::atomic<std::size_t> active_sessions_{ 0 };
    };

#define G_LUA_DEBUGGER lua_debugger::get_instance()
