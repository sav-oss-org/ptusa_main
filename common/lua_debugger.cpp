#include "lua_debugger.h"

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iomanip>
#include <limits>
#include <locale>
#include <random>
#include <set>
#include <sstream>

#include "dtime.h"
#include "lua_manager.h"
#include "PAC_info.h"
#include "tcp_cmctr.h"
#include "tech_def.h"

namespace
    {
    constexpr std::size_t MAX_EXPRESSION_LENGTH = 1024;
    constexpr std::size_t MAX_CHART_STRING_LENGTH = 128;
    constexpr std::size_t MAX_MESSAGE_LENGTH = 1024;
    // The response frame has a 16-bit payload length, including the NUL.
    constexpr std::size_t MAX_RESPONSE_LENGTH = ( std::min<std::size_t> )(
        tcp_communicator::BUFSIZE - 6,
        ( std::numeric_limits<std::uint16_t>::max )() - 1 );

    std::string request_text( long len, const unsigned char* data,
        bool trim_nul = true )
        {
        if ( len <= 1 ) return {};

        std::string result( reinterpret_cast<const char*>( data + 1 ),
            static_cast<std::size_t>( len - 1 ) );
        if ( trim_nul )
            while ( !result.empty() && result.back() == '\0' )
                result.pop_back();
        return result;
        }

    bool split_session_request( const std::string& request,
        std::string& session_id, std::string& body )
        {
        const auto separator = request.find( '\n' );
        if ( separator == std::string::npos ) return false;
        session_id = request.substr( 0, separator );
        if ( !session_id.empty() && session_id.back() == '\r' )
            session_id.pop_back();
        body = request.substr( separator + 1 );
        return !session_id.empty();
        }

    constexpr std::size_t BROWSE_PAGE_SIZE = 128;
    constexpr std::size_t BROWSE_SCAN_LIMIT = 4096;
    constexpr std::size_t BROWSE_VALUE_STRING_LENGTH = 256;
    constexpr std::size_t BROWSE_NAME_LENGTH = 512;
    constexpr std::size_t BROWSE_INHERITANCE_DEPTH = 16;
    constexpr std::size_t MAX_TARGET_LENGTH = 1024;
    /// Reserved space for the JSON envelope around browse entries.
    constexpr std::size_t BROWSE_RESPONSE_MARGIN = 2048;

    int abs_index( lua_State* state, int index )
        {
        return index > 0 || index <= LUA_REGISTRYINDEX ? index :
            lua_gettop( state ) + index + 1;
        }

    /// Returns the length of the strictly valid UTF-8 sequence starting at
    /// source[i], or 0 for invalid input. Rejects overlong forms,
    /// surrogates and code points above U+10FFFF.
    std::size_t utf8_sequence_length( const std::string& source,
        std::size_t i )
        {
        const auto lead = static_cast<unsigned char>( source[ i ] );
        if ( lead < 0x80 ) return 1;
        std::size_t length = 0;
        if ( lead >= 0xC2 && lead <= 0xDF ) length = 2;
        else if ( lead >= 0xE0 && lead <= 0xEF ) length = 3;
        else if ( lead >= 0xF0 && lead <= 0xF4 ) length = 4;
        else return 0;
        if ( i + length > source.size() ) return 0;
        const auto second = static_cast<unsigned char>( source[ i + 1 ] );
        unsigned char low = 0x80, high = 0xBF;
        if ( lead == 0xE0 ) low = 0xA0;
        else if ( lead == 0xED ) high = 0x9F;
        else if ( lead == 0xF0 ) low = 0x90;
        else if ( lead == 0xF4 ) high = 0x8F;
        if ( second < low || second > high ) return 0;
        for ( std::size_t k = 2; k < length; ++k )
            {
            const auto ch =
                static_cast<unsigned char>( source[ i + k ] );
            if ( ( ch & 0xC0 ) != 0x80 ) return 0;
            }
        return length;
        }

    /// Replaces bytes that are not valid UTF-8 with Lua-style decimal
    /// escapes so that JSON output stays valid UTF-8.
    std::string utf8_sanitize( const std::string& source )
        {
        std::string result;
        result.reserve( source.size() );
        for ( std::size_t i = 0; i < source.size(); )
            {
            const std::size_t length = utf8_sequence_length( source, i );
            if ( length != 0 )
                {
                result.append( source, i, length );
                i += length;
                continue;
                }
            char escaped[ 5 ];
            std::snprintf( escaped, sizeof escaped,
                "\\%03u", static_cast<unsigned char>( source[ i ] ) );
            result += escaped;
            i++;
            }
        return result;
        }

    /// Bounds a display text to at most `limit` bytes without splitting
    /// a UTF-8 sequence; appends "..." when truncated.
    std::string bounded_text( const std::string& source,
        std::size_t limit )
        {
        if ( source.size() <= limit ) return source;
        std::size_t boundary = 0;
        std::size_t i = 0;
        while ( i < source.size() )
            {
            const std::size_t length = utf8_sequence_length( source, i );
            if ( length == 0 || i + length > limit ) break;
            i += length;
            boundary = i;
            }
        return source.substr( 0, boundary ) + "...";
        }

    /// True when a Lua string on the stack exceeds the preview limit.
    bool stack_string_truncated( lua_State* state, int index,
        std::size_t limit )
        {
        if ( lua_type( state, index ) != LUA_TSTRING ) return false;
        std::size_t length = 0;
        lua_tolstring( state, index, &length );
        return length > limit;
        }

    /// Slices raw string bytes to at most `limit` bytes at a complete
    /// UTF-8 boundary; bytes that are not valid UTF-8 count as one byte
    /// each and are escaped later by utf8_sanitize. Sets `truncated`
    /// when any byte was dropped.
    std::string utf8_preview( const char* text, std::size_t length,
        std::size_t limit, bool& truncated )
        {
        truncated = length > limit;
        if ( !truncated ) return std::string( text, length );
        // A few bytes past the limit suffice to see whether the byte at
        // the cut completes a valid sequence.
        const std::string view( text,
            ( std::min )( length, limit + 4 ) );
        std::size_t boundary = 0;
        for ( std::size_t i = 0; i < view.size(); )
            {
            const std::size_t sequence = utf8_sequence_length( view, i );
            const std::size_t step = sequence ? sequence : 1;
            if ( i + step > limit ) break;
            i += step;
            boundary = i;
            }
        return view.substr( 0, boundary );
        }

    /// True when the serialized preview of the string at `index`
    /// carries decimal escapes for non-UTF-8 bytes and therefore cannot
    /// be written back verbatim.
    bool stack_string_lossy( lua_State* state, int index,
        std::size_t limit )
        {
        if ( lua_type( state, index ) != LUA_TSTRING ) return false;
        std::size_t length = 0;
        const char* text = lua_tolstring( state, index, &length );
        bool truncated = false;
        const std::string preview =
            utf8_preview( text, length, limit, truncated );
        for ( std::size_t i = 0; i < preview.size(); )
            {
            const std::size_t sequence =
                utf8_sequence_length( preview, i );
            if ( sequence == 0 ) return true;
            i += sequence;
            }
        return false;
        }

    /// Quotes a Lua string key for use inside a [] selector. Valid UTF-8
    /// sequences pass through; other bytes become decimal escapes so the
    /// literal stays valid UTF-8 (and thus valid JSON) on the wire.
    std::string lua_quote( const std::string& source )
        {
        std::string out = "\"";
        for ( std::size_t i = 0; i < source.size(); )
            {
            const auto ch = static_cast<unsigned char>( source[ i ] );
            if ( ch >= 0x80 )
                {
                const std::size_t length =
                    utf8_sequence_length( source, i );
                if ( length != 0 )
                    {
                    out.append( source, i, length );
                    i += length;
                    }
                else
                    {
                    char escaped[ 5 ];
                    std::snprintf( escaped, sizeof escaped,
                        "\\%03u", ch );
                    out += escaped;
                    i++;
                    }
                continue;
                }
            switch ( ch )
                {
                case '"': out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\a': out += "\\a"; break;
                case '\b': out += "\\b"; break;
                case '\f': out += "\\f"; break;
                case '\n': out += "\\n"; break;
                case '\r': out += "\\r"; break;
                case '\t': out += "\\t"; break;
                case '\v': out += "\\v"; break;
                default:
                    if ( ch < 0x20 || ch == 0x7f )
                        {
                        char escaped[ 5 ];
                        std::snprintf( escaped, sizeof escaped, "\\%03u", ch );
                        out += escaped;
                        }
                    else
                        {
                        out += static_cast<char>( ch );
                        }
                }
            i++;
            }
        out += '"';
        return out;
        }

    std::string format_lua_number( double number )
        {
        std::ostringstream out;
        out.imbue( std::locale::classic() );
        out << std::setprecision(
            std::numeric_limits<lua_Number>::max_digits10 ) << number;
        return out.str();
        }

    bool is_identifier_char( char ch, bool first )
        {
        const auto uch = static_cast<unsigned char>( ch );
        return ( uch >= 'a' && uch <= 'z' ) ||
            ( uch >= 'A' && uch <= 'Z' ) || uch == '_' ||
            ( !first && uch >= '0' && uch <= '9' );
        }

    bool is_lua_keyword( const std::string& text )
        {
        static const std::set<std::string> keywords = {
            "and", "break", "do", "else", "elseif", "end", "false", "for",
            "function", "if", "in", "local", "nil", "not", "or", "repeat",
            "return", "then", "true", "until", "while" };
        return keywords.count( text ) != 0;
        }

    /// Decimal/scientific Lua number literal without a leading '+'.
    bool is_number_literal( const std::string& text )
        {
        std::size_t i = 0;
        if ( i < text.size() && text[ i ] == '-' ) i++;
        bool digits = false;
        while ( i < text.size() && text[ i ] >= '0' && text[ i ] <= '9' )
            { digits = true; i++; }
        if ( i < text.size() && text[ i ] == '.' )
            {
            i++;
            while ( i < text.size() && text[ i ] >= '0' && text[ i ] <= '9' )
                { digits = true; i++; }
            }
        if ( !digits ) return false;
        if ( i < text.size() && ( text[ i ] == 'e' || text[ i ] == 'E' ) )
            {
            i++;
            if ( i < text.size() &&
                ( text[ i ] == '-' || text[ i ] == '+' ) ) i++;
            bool exponent = false;
            while ( i < text.size() && text[ i ] >= '0' && text[ i ] <= '9' )
                { exponent = true; i++; }
            if ( !exponent ) return false;
            }
        return i == text.size();
        }

    /// Decodes a Lua quoted string literal starting at pos (a quote char).
    /// On success pos is one past the closing quote.
    bool parse_string_literal( const std::string& text, std::size_t& pos,
        std::string& decoded )
        {
        const char quote = text[ pos++ ];
        decoded.clear();
        while ( pos < text.size() )
            {
            const auto ch = static_cast<unsigned char>( text[ pos ] );
            if ( ch == static_cast<unsigned char>( quote ) )
                {
                pos++;
                return true;
                }
            if ( ch == '\\' )
                {
                pos++;
                if ( pos >= text.size() ) return false;
                const auto esc = static_cast<unsigned char>( text[ pos++ ] );
                switch ( esc )
                    {
                    case 'a': decoded += '\a'; break;
                    case 'b': decoded += '\b'; break;
                    case 'f': decoded += '\f'; break;
                    case 'n': decoded += '\n'; break;
                    case 'r': decoded += '\r'; break;
                    case 't': decoded += '\t'; break;
                    case 'v': decoded += '\v'; break;
                    case '\\': decoded += '\\'; break;
                    case '"': decoded += '"'; break;
                    case '\'': decoded += '\''; break;
                    case '\n': decoded += '\n'; break;
                    default:
                        if ( esc >= '0' && esc <= '9' )
                            {
                            int numeric = esc - '0';
                            for ( int digit = 0; digit < 2 &&
                                pos < text.size() && text[ pos ] >= '0' &&
                                text[ pos ] <= '9'; ++digit )
                                {
                                numeric = numeric * 10 + ( text[ pos++ ] - '0' );
                                }
                            if ( numeric > 255 ) return false;
                            decoded += static_cast<char>( numeric );
                            break;
                            }
                        return false;
                    }
                continue;
                }
            if ( ch < 0x20 || ch == 0x7f ) return false;
            decoded += static_cast<char>( ch );
            pos++;
            }
        return false;
        }

    struct assignment_target
        {
        bool valid = false;
        /// Container expression, empty for a bare global identifier.
        std::string parent;
        /// Decoded key of the last selector.
        std::string key;
        bool key_is_string = false;
        };

    /// Validates a safe assignable Lua path:
    /// identifier followed by .name or [string/number/boolean] selectors.
    assignment_target parse_assignment_target( const std::string& source )
        {
        assignment_target result;
        if ( source.empty() || source.size() > MAX_TARGET_LENGTH )
            return result;
        std::size_t pos = 0;
        while ( pos < source.size() &&
            is_identifier_char( source[ pos ], pos == 0 ) ) pos++;
        if ( pos == 0 ||
            is_lua_keyword( source.substr( 0, pos ) ) ) return result;
        std::size_t last_selector = pos;
        while ( pos < source.size() )
            {
            last_selector = pos;
            if ( source[ pos ] == '.' )
                {
                pos++;
                const auto start = pos;
                while ( pos < source.size() &&
                    is_identifier_char( source[ pos ], pos == start ) ) pos++;
                if ( pos == start ||
                    is_lua_keyword( source.substr( start, pos - start ) ) )
                    return result;
                result.key = source.substr( start, pos - start );
                result.key_is_string = true;
                continue;
                }
            if ( source[ pos ] != '[' ) return result;
            pos++;
            if ( pos >= source.size() ) return result;
            if ( source[ pos ] == '"' || source[ pos ] == '\'' )
                {
                if ( !parse_string_literal( source, pos, result.key ) )
                    return result;
                result.key_is_string = true;
                }
            else
                {
                const auto close = source.find( ']', pos );
                if ( close == std::string::npos ) return result;
                const std::string token = source.substr( pos, close - pos );
                const bool boolean = token == "true" || token == "false";
                if ( !boolean && !is_number_literal( token ) ) return result;
                result.key = token;
                result.key_is_string = false;
                pos = close;
                }
            if ( pos >= source.size() || source[ pos ] != ']' ) return result;
            pos++;
            }
        result.valid = true;
        if ( pos != last_selector )
            result.parent = source.substr( 0, last_selector );
        return result;
        }

    /// True when the stack top is a dedicated userdata peer table (tolua
    /// ubox on Lua 5.1). The registry or the plain global table means
    /// "no peer": treating _G as one would list unrelated globals and
    /// peer writes would hit global variables, not the userdata.
    bool is_dedicated_peer( lua_State* state )
        {
        return lua_istable( state, -1 ) &&
            !lua_rawequal( state, -1, LUA_REGISTRYINDEX ) &&
            !lua_rawequal( state, -1, LUA_GLOBALSINDEX );
        }

    /// True when the userdata allows writing `key`: an existing peer field
    /// or a .set function in the metatable chain (tolua convention).
    bool userdata_writable( lua_State* state, int index,
        const std::string& key )
        {
        index = abs_index( state, index );
        lua_getfenv( state, index );
        if ( is_dedicated_peer( state ) )
            {
            lua_pushlstring( state, key.data(), key.size() );
            lua_rawget( state, -2 );
            const bool found = !lua_isnil( state, -1 );
            lua_pop( state, 2 );
            if ( found ) return true;
            }
        else
            {
            lua_pop( state, 1 );
            }
        if ( !lua_getmetatable( state, index ) ) return false;
        for ( int depth = 0; depth < 16; ++depth )
            {
            lua_pushlstring( state, ".set", 4 );
            lua_rawget( state, -2 );
            bool writable = false;
            if ( lua_istable( state, -1 ) )
                {
                lua_pushlstring( state, key.data(), key.size() );
                lua_rawget( state, -2 );
                writable = lua_isfunction( state, -1 );
                lua_pop( state, 1 );
                }
            lua_pop( state, 1 );
            if ( writable )
                {
                lua_pop( state, 1 );
                return true;
                }
            if ( !lua_getmetatable( state, -1 ) )
                {
                lua_pop( state, 1 );
                return false;
                }
            lua_remove( state, -2 );
            }
        lua_pop( state, 1 );
        return false;
        }
    }

lua_debugger* lua_debugger::get_instance()
    {
    static lua_debugger instance;
    return &instance;
    }

bool lua_debugger::value::operator==( const value& rhs ) const
    {
    return ok == rhs.ok && type == rhs.type && json == rhs.json;
    }

std::string lua_debugger::json_quote( const std::string& source )
    {
    std::ostringstream out;
    out << '"';
    for ( unsigned char ch : source )
        {
        switch ( ch )
            {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\b': out << "\\b"; break;
            case '\f': out << "\\f"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default:
                if ( ch < 0x20 )
                    {
                    out << "\\u" << std::hex << std::setw( 4 )
                        << std::setfill( '0' ) << static_cast<int>( ch )
                        << std::dec;
                    }
                else
                    {
                    out << static_cast<char>( ch );
                    }
            }
        }
    out << '"';
    return out.str();
    }

lua_debugger::value lua_debugger::value_from_stack(
    lua_State* state, int index, std::size_t max_string_length ) const
    {
    value result;
    result.ok = true;
    result.type = lua_typename( state, lua_type( state, index ) );

    switch ( lua_type( state, index ) )
        {
        case LUA_TNIL:
            result.json = "null";
            break;
        case LUA_TBOOLEAN:
            result.json = lua_toboolean( state, index ) ? "true" : "false";
            break;
        case LUA_TNUMBER:
            {
            const auto number = lua_tonumber( state, index );
            if ( !std::isfinite( number ) )
                {
                result.json = json_quote( std::isnan( number ) ? "nan" :
                    number > 0 ? "infinity" : "-infinity" );
                break;
                }
            std::ostringstream out;
            out.imbue( std::locale::classic() );
            out << std::setprecision(
                std::numeric_limits<lua_Number>::max_digits10 ) << number;
            result.json = out.str();
            break;
            }
        case LUA_TSTRING:
            {
            std::size_t length = 0;
            const char* text = lua_tolstring( state, index, &length );
            bool truncated = false;
            std::string preview = utf8_preview( text, length,
                max_string_length, truncated );
            if ( truncated ) preview += "...";
            result.json = json_quote( utf8_sanitize( preview ) );
            break;
            }
        default:
            result.json = json_quote( "<" + result.type + ">" );
            break;
        }
    return result;
    }

lua_debugger::value lua_debugger::evaluate_source(
    const std::string& source ) const
    {
    return execute_source( "return (" + source + ")" );
    }

lua_debugger::value lua_debugger::execute_source(
    const std::string& chunk ) const
    {
    auto* state = G_LUA_MANAGER->get_Lua();
    if ( !state ) return { false, "error", json_quote( "Lua is not initialized" ) };

    const int stack_top = lua_gettop( state );
    if ( luaL_loadbuffer( state, chunk.data(), chunk.size(), "lua debugger" ) ||
        lua_pcall( state, 0, 1, 0 ) )
        {
        const char* error = lua_tostring( state, -1 );
        value result{ false, "error", json_quote( utf8_sanitize(
            error ? error : "Unknown Lua error" ) ) };
        lua_settop( state, stack_top );
        return result;
        }

    value result = value_from_stack( state, -1, MAX_RESPONSE_LENGTH / 2 );
    lua_settop( state, stack_top );
    return result;
    }

lua_debugger::value lua_debugger::evaluate_ref( int lua_ref ) const
    {
    auto* state = G_LUA_MANAGER->get_Lua();
    const int stack_top = lua_gettop( state );
    lua_rawgeti( state, LUA_REGISTRYINDEX, lua_ref );
    if ( lua_pcall( state, 0, 1, 0 ) )
        {
        const char* error = lua_tostring( state, -1 );
        value result{ false, "error", json_quote( utf8_sanitize(
            error ? error : "Unknown Lua error" ) ) };
        lua_settop( state, stack_top );
        return result;
        }

    value result = value_from_stack( state, -1, MAX_CHART_STRING_LENGTH );
    lua_settop( state, stack_top );
    return result;
    }

std::string lua_debugger::create_session()
    {
    if ( sessions_.size() >= MAX_SESSIONS )
        return R"({"ok":false,"error":"Too many debugger sessions"})";

    if ( !state_ ) state_ = G_LUA_MANAGER->get_Lua();
    if ( !state_ ) return R"({"ok":false,"error":"Lua is not initialized"})";

    std::string session_id;
    do
        {
        std::random_device random;
        const auto id_value =
            ( static_cast<std::uint64_t>( random() ) << 32 ) ^
            static_cast<std::uint64_t>( random() ) ^ next_session_id_++;
        std::ostringstream id_stream;
        id_stream << std::hex << std::setw( 16 ) << std::setfill( '0' )
            << id_value;
        session_id = id_stream.str();
        }
    while ( sessions_.find( session_id ) != sessions_.end() );

    const auto controller_time_millisec = get_millisec();
    const auto controller_time_unix_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch() ).count();
    auto& new_session = sessions_[ session_id ];
    new_session.last_access_ms = controller_time_millisec;
    active_sessions_.store( sessions_.size(), std::memory_order_relaxed );
    {
        std::lock_guard<std::mutex> lock( messages_mutex_ );
        new_session.next_message_id = next_message_id_;
    }
    return R"({"ok":true,"session_id":)" + json_quote( session_id ) +
        R"(,"timeout_ms":)" + std::to_string( SESSION_TIMEOUT_MS ) +
        R"(,"controller_time_unix_ms":)" +
        std::to_string( controller_time_unix_ms ) +
        R"(,"controller_time_millisec":)" +
        std::to_string( controller_time_millisec ) + "}";
    }

std::string lua_debugger::set_expressions( session& target,
    const std::string& request )
    {
    auto* state = G_LUA_MANAGER->get_Lua();
    if ( !state ) return R"({"ok":false,"error":"Lua is not initialized"})";

    std::vector<expression> replacement;
    std::istringstream input( request );
    std::string source;
    while ( std::getline( input, source ) )
        {
        if ( !source.empty() && source.back() == '\r' ) source.pop_back();
        if ( source.empty() ) continue;
        if ( source.size() > MAX_EXPRESSION_LENGTH )
            {
            for ( auto& item : replacement )
                luaL_unref( state, LUA_REGISTRYINDEX, item.lua_ref );
            return R"({"ok":false,"error":"Expression is too long"})";
            }
        if ( replacement.size() >= MAX_EXPRESSIONS )
            {
            for ( auto& item : replacement )
                luaL_unref( state, LUA_REGISTRYINDEX, item.lua_ref );
            return R"({"ok":false,"error":"Too many expressions"})";
            }

        const std::string chunk = "return (" + source + ")";
        if ( luaL_loadbuffer( state, chunk.data(), chunk.size(), "lua chart" ) )
            {
            const char* error = lua_tostring( state, -1 );
            const std::string response = R"({"ok":false,"expression":)" +
                json_quote( source ) + R"(,"error":)" +
                json_quote( utf8_sanitize(
                    error ? error : "Unknown Lua error" ) ) + "}";
            lua_pop( state, 1 );
            for ( auto& item : replacement )
                luaL_unref( state, LUA_REGISTRYINDEX, item.lua_ref );
            return response;
            }

        expression item;
        item.source = source;
        item.lua_ref = luaL_ref( state, LUA_REGISTRYINDEX );
        replacement.push_back( std::move( item ) );
        }

    release_expressions( target );
    target.expressions = std::move( replacement );
    return R"({"ok":true,"count":)" +
        std::to_string( target.expressions.size() ) + "}";
    }

std::string lua_debugger::browse_variables(
    const std::string& request ) const
    {
    auto* state = G_LUA_MANAGER->get_Lua();
    if ( !state ) return R"({"ok":false,"error":"Lua is not initialized"})";

    const auto separator = request.rfind( '\n' );
    if ( separator == std::string::npos )
        return R"({"ok":false,"error":"Missing browse offset"})";
    const std::string expression = request.substr( 0, separator );
    const std::string offset_text = request.substr( separator + 1 );
    if ( expression.empty() )
        return R"({"ok":false,"error":"Missing expression"})";
    if ( expression.size() > MAX_EXPRESSION_LENGTH )
        return R"({"ok":false,"error":"Expression is too long"})";
    std::size_t offset = 0;
    const auto parsed = std::from_chars( offset_text.data(),
        offset_text.data() + offset_text.size(), offset );
    if ( parsed.ec != std::errc{} ||
        parsed.ptr != offset_text.data() + offset_text.size() )
        return R"({"ok":false,"error":"Invalid browse offset"})";

    const int stack_top = lua_gettop( state );
    const std::string chunk = "return (" + expression + ")";
    if ( luaL_loadbuffer( state, chunk.data(), chunk.size(),
        "lua debugger" ) || lua_pcall( state, 0, 1, 0 ) )
        {
        const char* error = lua_tostring( state, -1 );
        const std::string response = R"({"ok":false,"expression":)" +
            json_quote( expression ) + R"(,"type":"error","error":)" +
            json_quote( utf8_sanitize(
                error ? error : "Unknown Lua error" ) ) + "}";
        lua_settop( state, stack_top );
        return response;
        }

    // Object identity is separate from the display preview. Clients compare
    // it only with ancestors of the current snapshot, never across sessions.
    auto object_id = [&]( int index ) -> std::string
        {
        const int type = lua_type( state, index );
        if ( type != LUA_TTABLE && type != LUA_TUSERDATA ) return {};
        return std::string( lua_typename( state, type ) ) + ":" +
            std::to_string( reinterpret_cast<std::uintptr_t>(
                lua_topointer( state, index ) ) );
        };

    struct entry
        {
        std::string name;
        std::string expression;
        std::string type;
        std::string json;
        std::string object_id;
        bool expandable = false;
        bool writable = false;
        bool value_truncated = false;
        /// The preview escaped non-UTF-8 bytes and cannot round-trip.
        bool value_lossy = false;
        /// Deferred .get call resolved only for the requested page.
        int getter_ref = LUA_NOREF;
        std::string getter_key;
        /// Ordering within equal names: string, number, boolean, other.
        int order = 3;
        };

    struct context
        {
        std::vector<entry> entries;
        std::set<std::string> seen;
        std::set<const void*> visited;
        std::set<std::string> setters;
        std::size_t scanned = 0;
        bool truncated = false;
        bool assignable = false;
        std::string root;
        int unsupported = 0;
        } ctx;
    ctx.assignable = parse_assignment_target( expression ).valid;
    ctx.root = expression;

    auto scalar = []( const std::string& type )
        { return type == "number" || type == "string" || type == "boolean"; };
    auto child_expression = [&]( const std::string& selector )
        {
        std::string child = ctx.assignable ?
            ctx.root + selector : "(" + ctx.root + ")" + selector;
        return child.size() > MAX_TARGET_LENGTH ? std::string() : child;
        };
    auto add_entry = [&]( const entry& item )
        { ctx.entries.push_back( item ); };

    // Adds an entry for the key/value pair currently on the stack
    // (key at -2, value at -1). Applies to tables and userdata peers.
    std::function<void( int, bool )> add_pair = [&]( int container_type,
        bool peer )
        {
        const int key_type = lua_type( state, -2 );
        entry item;
        std::string dedup;
        switch ( key_type )
            {
            case LUA_TSTRING:
                {
                std::size_t length = 0;
                const char* text = lua_tolstring( state, -2, &length );
                const std::string key( text, length );
                dedup = "s\x01" + key;
                if ( peer && ( key.empty() || key[ 0 ] == '.' ||
                    key.compare( 0, 2, "__" ) == 0 ) )
                    {
                    lua_pop( state, 1 );
                    return;
                    }
                {
                const std::string full_name = utf8_sanitize( key );
                if ( full_name.size() > BROWSE_NAME_LENGTH )
                    ctx.truncated = true;
                item.name = bounded_text( full_name,
                    BROWSE_NAME_LENGTH );
                }
                item.expression = child_expression( "[" + lua_quote( key ) + "]" );
                item.order = 0;
                break;
                }
            case LUA_TNUMBER:
                {
                const double key = lua_tonumber( state, -2 );
                std::uint64_t bits = 0;
                static_assert( sizeof bits == sizeof key );
                std::memcpy( &bits, &key, sizeof bits );
                dedup = "n\x01" + std::to_string( bits );
                item.name = format_lua_number( key );
                if ( std::isfinite( key ) )
                    {
                    item.expression =
                        child_expression( "[" + item.name + "]" );
                    }
                item.order = 1;
                break;
                }
            case LUA_TBOOLEAN:
                {
                const bool key = lua_toboolean( state, -2 ) != 0;
                dedup = key ? "b\x011" : "b\x010";
                item.name = key ? "true" : "false";
                item.expression =
                    child_expression( key ? "[true]" : "[false]" );
                item.order = 2;
                break;
                }
            default:
                {
                dedup = "p\x01" + std::to_string(
                    reinterpret_cast<std::uintptr_t>(
                        lua_topointer( state, -2 ) ) );
                item.name = "<" + std::string(
                    lua_typename( state, key_type ) ) + " key " +
                    std::to_string( ++ctx.unsupported ) + ">";
                break;
                }
            }
        if ( !ctx.seen.insert( dedup ).second )
            {
            lua_pop( state, 1 );
            return;
            }
        item.value_truncated = stack_string_truncated( state, -1,
            BROWSE_VALUE_STRING_LENGTH );
        item.value_lossy = stack_string_lossy( state, -1,
            BROWSE_VALUE_STRING_LENGTH );
        const value current = value_from_stack( state, -1,
            BROWSE_VALUE_STRING_LENGTH );
        item.type = current.type;
        item.json = current.json;
        item.object_id = object_id( -1 );
        item.expandable = !item.expression.empty() &&
            ( current.type == "table" || current.type == "userdata" );
        if ( container_type == LUA_TTABLE )
            {
            item.writable = ctx.assignable && !item.expression.empty() &&
                item.order <= 2 && scalar( current.type );
            }
        else
            {
            // Peer fields are real userdata fields; only named scalar
            // fields can be written through the assignment protocol.
            item.writable = ctx.assignable && !item.expression.empty() &&
                item.order == 0 && scalar( current.type );
            }
        // Paths over the protocol limit cannot be browsed or written;
        // present the entry as unaddressable instead of fabricating a
        // request the server would reject.
        if ( item.expression.size() > MAX_TARGET_LENGTH )
            {
            item.expression.clear();
            ctx.truncated = true;
            }
        item.expandable = item.expandable && !item.expression.empty();
        item.writable = item.writable && !item.expression.empty();
        add_entry( item );
        lua_pop( state, 1 );
        };

    std::function<void( int, int )> collect_table = [&]( int index,
        int depth )
        {
        index = abs_index( state, index );
        ctx.visited.insert( lua_topointer( state, index ) );
        lua_pushnil( state );
        while ( lua_next( state, index ) != 0 )
            {
            if ( ++ctx.scanned > BROWSE_SCAN_LIMIT )
                {
                ctx.truncated = true;
                lua_pop( state, 2 );
                break;
                }
            add_pair( LUA_TTABLE, false );
            }
        if ( depth >= BROWSE_INHERITANCE_DEPTH ||
            !lua_getmetatable( state, index ) ) return;
        lua_pushstring( state, "__index" );
        lua_rawget( state, -2 );
        if ( lua_istable( state, -1 ) )
            {
            if ( depth + 1 >= BROWSE_INHERITANCE_DEPTH )
                ctx.truncated = true;
            else if ( !ctx.visited.count( lua_topointer( state, -1 ) ) )
                collect_table( lua_gettop( state ), depth + 1 );
            }
        lua_pop( state, 2 );
        };

    std::function<void( int )> collect_userdata = [&]( int index )
        {
        index = abs_index( state, index );

        // Actual fields live in a dedicated peer table (tolua ubox on
        // Lua 5.1 is the userdata environment); the registry or _G
        // mean "no peer".
        lua_getfenv( state, index );
        const bool has_peer = is_dedicated_peer( state );
        if ( has_peer )
            {
            lua_pushnil( state );
            while ( lua_next( state, -2 ) != 0 )
                {
                if ( ++ctx.scanned > BROWSE_SCAN_LIMIT )
                    {
                    ctx.truncated = true;
                    lua_pop( state, 2 );
                    break;
                    }
                add_pair( LUA_TUSERDATA, true );
                }
            }
        lua_pop( state, 1 );

        // Writable .set properties are checked against the whole
        // metatable-of-metatable inheritance chain before enumerating.
        auto& setters = ctx.setters;
        std::set<const void*> set_chain;
        if ( lua_getmetatable( state, index ) )
            {
            bool has_metatable = true;
            for ( int depth = 0; has_metatable &&
                depth < BROWSE_INHERITANCE_DEPTH &&
                set_chain.insert( lua_topointer( state, -1 ) ).second;
                ++depth )
                {
                lua_pushlstring( state, ".set", 4 );
                lua_rawget( state, -2 );
                if ( lua_istable( state, -1 ) )
                    {
                    lua_pushnil( state );
                    while ( lua_next( state, -2 ) != 0 )
                        {
                        if ( ++ctx.scanned > BROWSE_SCAN_LIMIT )
                            {
                            ctx.truncated = true;
                            lua_pop( state, 2 );
                            break;
                            }
                        if ( lua_type( state, -2 ) == LUA_TSTRING )
                            {
                            std::size_t length = 0;
                            const char* text =
                                lua_tolstring( state, -2, &length );
                            if ( lua_isfunction( state, -1 ) )
                                setters.emplace( text, length );
                            }
                        lua_pop( state, 1 );
                        }
                    }
                lua_pop( state, 1 );
                if ( lua_getmetatable( state, -1 ) ) lua_remove( state, -2 );
                else
                    {
                    lua_pop( state, 1 );
                    has_metatable = false;
                    }
                }
            }

        // Method fields and .get properties along the chain.
        std::set<const void*> method_chain;
        if ( lua_getmetatable( state, index ) )
            {
            bool has_metatable = true;
            for ( int depth = 0; has_metatable &&
                depth < BROWSE_INHERITANCE_DEPTH &&
                method_chain.insert( lua_topointer( state, -1 ) ).second;
                ++depth )
                {
                lua_pushnil( state );
                while ( lua_next( state, -2 ) != 0 )
                    {
                    if ( ++ctx.scanned > BROWSE_SCAN_LIMIT )
                        {
                        ctx.truncated = true;
                        lua_pop( state, 2 );
                        break;
                        }
                    if ( lua_type( state, -2 ) != LUA_TSTRING )
                        {
                        lua_pop( state, 1 );
                        continue;
                        }
                    std::size_t length = 0;
                    const char* text = lua_tolstring( state, -2, &length );
                    const std::string key( text, length );
                    if ( key.empty() || key[ 0 ] == '.' ||
                        key.compare( 0, 2, "__" ) == 0 )
                        {
                        lua_pop( state, 1 );
                        continue;
                        }
                    entry item;
                    const std::string full_name = utf8_sanitize( key );
                    if ( full_name.size() > BROWSE_NAME_LENGTH )
                        ctx.truncated = true;
                    item.name = bounded_text( full_name,
                        BROWSE_NAME_LENGTH );
                    item.expression =
                        child_expression( "[" + lua_quote( key ) + "]" );
                    if ( item.expression.size() > MAX_TARGET_LENGTH )
                        {
                        item.expression.clear();
                        ctx.truncated = true;
                        }
                    item.order = 0;
                    if ( ctx.seen.insert( "s\x01" + key ).second )
                        {
                        item.value_truncated = stack_string_truncated(
                            state, -1, BROWSE_VALUE_STRING_LENGTH );
                        item.value_lossy = stack_string_lossy(
                            state, -1, BROWSE_VALUE_STRING_LENGTH );
                        const value current = value_from_stack( state, -1,
                            BROWSE_VALUE_STRING_LENGTH );
                        item.type = current.type;
                        item.json = current.json;
                        item.object_id = object_id( -1 );
                        item.expandable = !item.expression.empty() &&
                            ( current.type == "table" ||
                                current.type == "userdata" );
                        add_entry( item );
                        }
                    lua_pop( state, 1 );
                    }

                // Exposed .get properties of this metatable. Getter
                // calls are deferred until enumeration finishes: a
                // getter may mutate the .get table while lua_next walks
                // it, and only the requested page needs values anyway.
                lua_pushlstring( state, ".get", 4 );
                lua_rawget( state, -2 );
                if ( lua_istable( state, -1 ) )
                    {
                    lua_pushnil( state );
                    while ( lua_next( state, -2 ) != 0 )
                        {
                        if ( ++ctx.scanned > BROWSE_SCAN_LIMIT )
                            {
                            ctx.truncated = true;
                            lua_pop( state, 2 );
                            break;
                            }
                        if ( lua_type( state, -2 ) != LUA_TSTRING )
                            {
                            lua_pop( state, 1 );
                            continue;
                            }
                        std::size_t key_length = 0;
                        const char* key_text =
                            lua_tolstring( state, -2, &key_length );
                        const std::string key( key_text, key_length );
                        if ( key.empty() || key[ 0 ] == '.' ||
                            key.compare( 0, 2, "__" ) == 0 ||
                            !ctx.seen.insert( "s\x01" + key ).second )
                            {
                            lua_pop( state, 1 );
                            continue;
                            }
                        entry item;
                        const std::string full_name =
                            utf8_sanitize( key );
                        if ( full_name.size() > BROWSE_NAME_LENGTH )
                            ctx.truncated = true;
                        item.name = bounded_text( full_name,
                            BROWSE_NAME_LENGTH );
                        item.expression = child_expression(
                            "[" + lua_quote( key ) + "]" );
                        if ( item.expression.size() > MAX_TARGET_LENGTH )
                            {
                            item.expression.clear();
                            ctx.truncated = true;
                            }
                        item.order = 0;
                        if ( lua_isfunction( state, -1 ) )
                            {
                            item.getter_ref = luaL_ref( state,
                                LUA_REGISTRYINDEX );
                            item.getter_key = key;
                            item.type = "function";
                            item.json = json_quote( "<function>" );
                            }
                        else if ( lua_istable( state, -1 ) )
                            {
                            // Internal C array exposed through .get.
                            item.object_id = object_id( -1 );
                            item.type = "table";
                            item.json = json_quote( "<table>" );
                            item.expandable = !item.expression.empty();
                            lua_pop( state, 1 );
                            }
                        else
                            {
                            lua_pop( state, 1 );
                            continue;
                            }
                        add_entry( item );
                        }
                    }
                lua_pop( state, 1 );
                if ( lua_getmetatable( state, -1 ) ) lua_remove( state, -2 );
                else
                    {
                    lua_pop( state, 1 );
                    has_metatable = false;
                    }
                }
            }
        };

    const int root_index = stack_top + 1;
    const int root_type = lua_type( state, root_index );
    const std::string root_type_name =
        lua_typename( state, root_type );
    std::string root_json;
    bool root_truncated = false;
    bool root_lossy = false;
    if ( root_type == LUA_TTABLE )
        {
        root_json = json_quote( "<table>" );
        collect_table( root_index, 0 );
        }
    else if ( root_type == LUA_TUSERDATA )
        {
        root_json = json_quote( "<userdata>" );
        collect_userdata( root_index );
        }
    else
        {
        root_truncated = stack_string_truncated( state, root_index,
            BROWSE_VALUE_STRING_LENGTH );
        root_lossy = stack_string_lossy( state, root_index,
            BROWSE_VALUE_STRING_LENGTH );
        root_json = value_from_stack( state, root_index,
            BROWSE_VALUE_STRING_LENGTH ).json;
        }

    std::stable_sort( ctx.entries.begin(), ctx.entries.end(),
        []( const entry& lhs, const entry& rhs )
        {
        if ( lhs.name != rhs.name ) return lhs.name < rhs.name;
        if ( lhs.order != rhs.order ) return lhs.order < rhs.order;
        return lhs.expression < rhs.expression;
        } );

    // Enumeration is finished: resolve the .get properties that land on
    // the requested page. Everything else keeps its collected ref only
    // until cleanup below.
    const std::size_t page_end = ( std::min<std::size_t> )(
        offset + BROWSE_PAGE_SIZE, ctx.entries.size() );
    for ( std::size_t i = offset; i < page_end; ++i )
        {
        auto& item = ctx.entries[ i ];
        if ( item.getter_ref == LUA_NOREF ) continue;
        lua_rawgeti( state, LUA_REGISTRYINDEX, item.getter_ref );
        lua_pushvalue( state, root_index );
        lua_pushlstring( state, item.getter_key.data(),
            item.getter_key.size() );
        if ( lua_pcall( state, 2, 1, 0 ) )
            {
            item.type = "error";
            const char* error = lua_tostring( state, -1 );
            item.json = json_quote( bounded_text( utf8_sanitize(
                error ? error : "Unknown Lua error" ),
                BROWSE_VALUE_STRING_LENGTH ) );
            }
        else
            {
            item.value_truncated = stack_string_truncated( state, -1,
                BROWSE_VALUE_STRING_LENGTH );
            item.value_lossy = stack_string_lossy( state, -1,
                BROWSE_VALUE_STRING_LENGTH );
            const value current = value_from_stack( state, -1,
                BROWSE_VALUE_STRING_LENGTH );
            item.type = current.type;
            item.json = current.json;
            item.object_id = object_id( -1 );
            item.expandable = !item.expression.empty() &&
                ( current.type == "table" || current.type == "userdata" );
            item.writable = ctx.assignable && !item.expression.empty() &&
                scalar( current.type ) &&
                ctx.setters.count( item.getter_key ) != 0;
            }
        lua_pop( state, 1 );
        }
    for ( auto& item : ctx.entries )
        {
        if ( item.getter_ref != LUA_NOREF )
            {
            luaL_unref( state, LUA_REGISTRYINDEX, item.getter_ref );
            item.getter_ref = LUA_NOREF;
            }
        }

    std::string response = R"({"ok":true,"expression":)" +
        json_quote( expression ) + R"(,"type":)" +
        json_quote( root_type_name ) + R"(,"value":)" + root_json +
        ( root_truncated ? R"(,"value_truncated":true)" : "" ) +
        ( root_lossy ? R"(,"value_lossy":true)" : "" ) +
        ( object_id( root_index ).empty() ? "" :
            R"(,"object_id":)" + json_quote( object_id( root_index ) ) ) +
        R"(,"entries":[)";
    std::size_t written = 0;
    for ( std::size_t i = offset; i < ctx.entries.size(); ++i )
        {
        const auto& item = ctx.entries[ i ];
        const std::string text = R"({"name":)" + json_quote( item.name ) +
            R"(,"expression":)" + json_quote( item.expression ) +
            R"(,"type":)" + json_quote( item.type ) + R"(,"value":)" +
            item.json + R"(,"expandable":)" +
            ( item.expandable ? "true" : "false" ) + R"(,"writable":)" +
            ( item.writable ? "true" : "false" ) +
            ( item.object_id.empty() ? "" :
                R"(,"object_id":)" + json_quote( item.object_id ) ) +
            ( item.value_truncated ? R"(,"value_truncated":true)" : "" ) +
            ( item.value_lossy ? R"(,"value_lossy":true)" : "" ) +
            "}";
        // Always emit at least one entry so next_offset can advance.
        if ( written && ( written >= BROWSE_PAGE_SIZE ||
            response.size() + text.size() + BROWSE_RESPONSE_MARGIN >
            MAX_RESPONSE_LENGTH ) )
            break;
        if ( written ) response += ',';
        response += text;
        written++;
        }
    const bool has_next = offset + written < ctx.entries.size();
    response += R"(],"offset":)" + std::to_string( offset ) +
        R"(,"next_offset":)" +
        ( has_next ? std::to_string( offset + written ) : "null" ) +
        R"(,"truncated":)" + ( ctx.truncated ? "true" : "false" ) + "}";
    lua_settop( state, stack_top );
    return response;
    }

std::string lua_debugger::set_variable( const std::string& request ) const
    {
    auto* state = G_LUA_MANAGER->get_Lua();
    if ( !state ) return R"({"ok":false,"error":"Lua is not initialized"})";

    const auto first = request.find( '\n' );
    const auto second = first == std::string::npos ? std::string::npos :
        request.find( '\n', first + 1 );
    if ( second == std::string::npos )
        return R"({"ok":false,"error":"Invalid assignment request"})";
    const std::string target = request.substr( 0, first );
    const std::string type_name =
        request.substr( first + 1, second - first - 1 );
    const std::string text = request.substr( second + 1 );
    if ( target.empty() )
        return R"({"ok":false,"error":"Missing expression"})";
    const auto parsed_target = parse_assignment_target( target );
    if ( !parsed_target.valid )
        return R"({"ok":false,"error":"Invalid assignment target"})";

    enum class kind_t { number, boolean, string, nil };
    kind_t kind;
    double number = 0;
    bool boolean = false;
    if ( type_name == "number" )
        {
        kind = kind_t::number;
        const auto parsed = std::from_chars( text.data(),
            text.data() + text.size(), number );
        if ( !is_number_literal( text ) || parsed.ec != std::errc{} ||
            parsed.ptr != text.data() + text.size() ||
            !std::isfinite( number ) )
            return R"({"ok":false,"error":"Invalid number value"})";
        }
    else if ( type_name == "boolean" )
        {
        kind = kind_t::boolean;
        if ( text == "true" ) boolean = true;
        else if ( text == "false" ) boolean = false;
        else return R"({"ok":false,"error":"Invalid boolean value"})";
        }
    else if ( type_name == "string" ) kind = kind_t::string;
    else if ( type_name == "nil" )
        {
        kind = kind_t::nil;
        if ( !text.empty() )
            return R"({"ok":false,"error":"Invalid nil value"})";
        }
    else
        {
        return R"({"ok":false,"error":"Invalid value type"})";
        }

    const int stack_top = lua_gettop( state );
    const std::string read_chunk = "return (" + target + ")";
    if ( luaL_loadbuffer( state, read_chunk.data(), read_chunk.size(),
        "lua debugger" ) || lua_pcall( state, 0, 1, 0 ) )
        {
        const char* error = lua_tostring( state, -1 );
        const std::string response = R"({"ok":false,"error":)" +
            json_quote( "Cannot resolve assignment target: " +
                utf8_sanitize( error ? error : "Unknown Lua error" ) ) + "}";
        lua_settop( state, stack_top );
        return response;
        }
    const int current_type = lua_type( state, -1 );
    lua_pop( state, 1 );
    if ( current_type == LUA_TNIL )
        {
        lua_settop( state, stack_top );
        return R"({"ok":false,"error":"Assignment target does not exist"})";
        }
    if ( current_type != LUA_TNUMBER && current_type != LUA_TBOOLEAN &&
        current_type != LUA_TSTRING )
        {
        lua_settop( state, stack_top );
        return R"({"ok":false,"error":"Assignment target is not a scalar variable"})";
        }

    if ( !parsed_target.parent.empty() )
        {
        const std::string parent_chunk =
            "return (" + parsed_target.parent + ")";
        if ( luaL_loadbuffer( state, parent_chunk.data(),
            parent_chunk.size(), "lua debugger" ) ||
            lua_pcall( state, 0, 1, 0 ) )
            {
            const char* error = lua_tostring( state, -1 );
            const std::string response = R"({"ok":false,"error":)" +
                json_quote( "Cannot resolve assignment container: " +
                    utf8_sanitize( error ? error : "Unknown Lua error" ) ) +
                "}";
            lua_settop( state, stack_top );
            return response;
            }
        const int parent_type = lua_type( state, -1 );
        if ( parent_type == LUA_TUSERDATA )
            {
            bool allowed = kind != kind_t::nil && parsed_target.key_is_string;
            if ( allowed )
                allowed = userdata_writable( state, -1, parsed_target.key );
            lua_pop( state, 1 );
            if ( !allowed )
                {
                lua_settop( state, stack_top );
                return R"({"ok":false,"error":"Property is read-only"})";
                }
            }
        else
            {
            lua_pop( state, 1 );
            if ( parent_type != LUA_TTABLE )
                {
                lua_settop( state, stack_top );
                return R"({"ok":false,"error":"Assignment container is not a table"})";
                }
            }
        }
    else if ( kind == kind_t::nil )
        {
        // A bare identifier targets a global variable; nil removes it.
        }

    // The typed argument reaches the assignment through ... so no local
    // name can shadow a legitimate global like `value`.
    const std::string write_chunk = target + " = ...; return " + target;
    if ( luaL_loadbuffer( state, write_chunk.data(), write_chunk.size(),
        "lua debugger" ) )
        {
        lua_settop( state, stack_top );
        return R"({"ok":false,"error":"Invalid assignment target"})";
        }
    switch ( kind )
        {
        case kind_t::number: lua_pushnumber( state, number ); break;
        case kind_t::boolean: lua_pushboolean( state, boolean ); break;
        case kind_t::string:
            lua_pushlstring( state, text.data(), text.size() );
            break;
        case kind_t::nil: lua_pushnil( state ); break;
        }
    if ( lua_pcall( state, 1, 1, 0 ) )
        {
        const char* error = lua_tostring( state, -1 );
        const std::string response = R"({"ok":false,"error":)" +
            json_quote( utf8_sanitize(
                error ? error : "Unknown Lua error" ) ) + "}";
        lua_settop( state, stack_top );
        return response;
        }
    const bool result_truncated = stack_string_truncated( state, -1,
        BROWSE_VALUE_STRING_LENGTH );
    const bool result_lossy = stack_string_lossy( state, -1,
        BROWSE_VALUE_STRING_LENGTH );
    const value result = value_from_stack( state, -1,
        BROWSE_VALUE_STRING_LENGTH );
    lua_settop( state, stack_top );
    return R"({"ok":true,"expression":)" + json_quote( target ) +
        R"(,"type":)" + json_quote( result.type ) + R"(,"value":)" +
        result.json +
        ( result_truncated ? R"(,"value_truncated":true)" : "" ) +
        ( result_lossy ? R"(,"value_lossy":true)" : "" ) +
        "}";
    }

void lua_debugger::evaluate()
    {
    auto* current_state = G_LUA_MANAGER->get_Lua();
    if ( !current_state || ( state_ && current_state != state_ ) ) return;

    const auto time_ms = get_millisec();
    expire_sessions();
    for ( auto& session_item : sessions_ )
        {
        for ( auto& item : session_item.second.expressions )
            {
            value current = evaluate_ref( item.lua_ref );
            if ( item.samples.empty() || !( item.samples.back().data == current ) )
                {
                item.samples.push_back( { time_ms, std::move( current ) } );
                if ( item.samples.size() > MAX_SAMPLES_PER_EXPRESSION )
                    item.samples.pop_front();
                }
            }
        }
    }

std::string lua_debugger::chart_data( const session& target ) const
    {
    std::string response = R"({"ok":true,"server_time_ms":)" +
        std::to_string( get_millisec() ) + R"(,"series":[)";
    bool first_expression = true;
    for ( const auto& item : target.expressions )
        {
        if ( !first_expression ) response += ',';
        first_expression = false;
        response += R"({"expression":)" + json_quote( item.source ) +
            R"(,"samples":[)";
        bool first_sample = true;
        for ( const auto& point : item.samples )
            {
            if ( !first_sample ) response += ',';
            first_sample = false;
            response += R"({"time_ms":)" + std::to_string( point.time_ms ) +
                R"(,"ok":)" + ( point.data.ok ? "true" : "false" ) +
                R"(,"type":)" + json_quote( point.data.type ) +
                R"(,"value":)" + point.data.json + "}";
            }
        response += "]}";
        }
    response += "]}";
    return response;
    }

void lua_debugger::publish_message( const char* source, int priority,
    const char* text )
    {
    if ( !source || !text ||
        active_sessions_.load( std::memory_order_relaxed ) == 0 ) return;

    message item;
    item.time_ms = get_millisec();
    item.source.assign( source );
    item.priority = priority;
    const auto text_length = std::strlen( text );
    item.text.assign( text,
        ( std::min )( text_length, MAX_MESSAGE_LENGTH ) );
    if ( text_length > MAX_MESSAGE_LENGTH ) item.text += "...";

    std::lock_guard<std::mutex> lock( messages_mutex_ );
    item.id = next_message_id_++;
    messages_.push_back( std::move( item ) );
    if ( messages_.size() > MAX_MESSAGES ) messages_.pop_front();
    }

std::string lua_debugger::message_data( session& target, std::size_t budget )
    {
    std::lock_guard<std::mutex> lock( messages_mutex_ );
    const auto first_available = messages_.empty() ? next_message_id_ :
        messages_.front().id;
    const auto dropped = target.next_message_id < first_available ?
        first_available - target.next_message_id : 0;
    if ( dropped ) target.next_message_id = first_available;

    std::string response = R"({"ok":true,"dropped":)" +
        std::to_string( dropped ) + R"(,"messages":[)";
    std::size_t count = 0;
    for ( const auto& item : messages_ )
        {
        if ( item.id < target.next_message_id ) continue;
        if ( count >= MAX_MESSAGES_PER_RESPONSE ) break;
        const auto entry = R"({"id":)" + std::to_string( item.id ) +
            R"(,"time_ms":)" + std::to_string( item.time_ms ) +
            R"(,"source":)" + json_quote( item.source ) +
            R"(,"priority":)" + std::to_string( item.priority ) +
            R"(,"text":)" + json_quote( item.text ) + "}";
        if ( response.size() + entry.size() + 3 > budget ) break;
        if ( count ) response += ',';
        response += entry;
        target.next_message_id = item.id + 1;
        count++;
        }
    response += "]}";
    return response;
    }

void lua_debugger::clear_samples( session& target )
    {
    for ( auto& item : target.expressions ) item.samples.clear();
    }

void lua_debugger::release_expressions( session& target )
    {
    auto* current_state = G_LUA_MANAGER->get_Lua();
    if ( state_ && state_ == current_state )
        {
        for ( auto& item : target.expressions )
            luaL_unref( state_, LUA_REGISTRYINDEX, item.lua_ref );
        }
    target.expressions.clear();
    }

void lua_debugger::expire_sessions()
    {
    for ( auto item = sessions_.begin(); item != sessions_.end(); )
        {
        if ( get_delta_millisec( item->second.last_access_ms ) >
            SESSION_TIMEOUT_MS )
            {
            release_expressions( item->second );
            item = sessions_.erase( item );
            }
        else
            {
            ++item;
            }
        }
    if ( sessions_.empty() ) state_ = nullptr;
    active_sessions_.store( sessions_.size(), std::memory_order_relaxed );
    }

void lua_debugger::reset( lua_State* state )
    {
    if ( state && state != state_ ) return;
    for ( auto& item : sessions_ ) release_expressions( item.second );
    sessions_.clear();
    state_ = nullptr;
    active_sessions_.store( 0, std::memory_order_relaxed );
    std::lock_guard<std::mutex> lock( messages_mutex_ );
    messages_.clear();
    next_message_id_ = 1;
    }

long lua_debugger::write_response( const std::string& response,
    unsigned char* outdata )
    {
    if ( response.size() > MAX_RESPONSE_LENGTH )
        {
        static constexpr char RESPONSE_TOO_LARGE[] =
            R"({"ok":false,"error":"Debugger response is too large"})";
        std::memcpy( outdata, RESPONSE_TOO_LARGE,
            sizeof( RESPONSE_TOO_LARGE ) );
        return sizeof( RESPONSE_TOO_LARGE );
        }

    std::memcpy( outdata, response.c_str(), response.size() + 1 );
    return static_cast<long>( response.size() + 1 );
    }

long lua_debugger::process_service( long len, unsigned char* data,
    unsigned char* outdata )
    {
    if ( len < 1 )
        return write_response( R"({"ok":false,"error":"Missing command"})",
            outdata );

    auto* debugger = get_instance();
    const auto command = static_cast<COMMAND>( data[ 0 ] );
    // Command 13 carries raw string data: trailing NUL bytes are payload.
    const std::string text = request_text( len, data,
        command != CMD_SET_VARIABLE );
    if ( command == CMD_CREATE_SESSION )
        return write_response( debugger->create_session(), outdata );

    std::string session_id;
    std::string body;
    if ( !split_session_request( text, session_id, body ) )
        return write_response(
            R"({"ok":false,"error":"Missing session id"})", outdata );

    const auto found = debugger->sessions_.find( session_id );
    if ( found == debugger->sessions_.end() )
        return write_response(
            R"({"ok":false,"error":"Invalid or expired session"})", outdata );

    auto& target = found->second;
    target.last_access_ms = get_millisec();
    switch ( command )
        {
        case CMD_EVALUATE:
        case CMD_EXECUTE:
            {
            if ( body.empty() )
                return write_response(
                    R"({"ok":false,"error":"Missing expression"})", outdata );
            if ( body.size() > ( command == CMD_EXECUTE ? 16384 :
                MAX_EXPRESSION_LENGTH ) )
                return write_response(
                    R"({"ok":false,"error":"Expression is too long"})",
                    outdata );
            const value result = command == CMD_EXECUTE ?
                debugger->execute_source( body ) :
                debugger->evaluate_source( body );
            const std::string response = R"({"ok":)" +
                std::string( result.ok ? "true" : "false" ) +
                R"(,"type":)" + json_quote( result.type ) +
                ( result.ok ? R"(,"value":)" : R"(,"error":)" ) +
                result.json + "}";
            return write_response( response, outdata );
            }
        case CMD_SET_CHART_EXPRESSIONS:
            return write_response(
                debugger->set_expressions( target, body ), outdata );
        case CMD_GET_CHART_DATA:
            return write_response( debugger->chart_data( target ), outdata );
        case CMD_CLEAR_CHART_DATA:
            debugger->clear_samples( target );
            return write_response( R"({"ok":true})", outdata );
        case CMD_CLOSE_SESSION:
            debugger->release_expressions( target );
            debugger->sessions_.erase( found );
            if ( debugger->sessions_.empty() ) debugger->state_ = nullptr;
            debugger->active_sessions_.store( debugger->sessions_.size(),
                std::memory_order_relaxed );
            return write_response( R"({"ok":true})", outdata );
        case CMD_KEEP_ALIVE:
            return write_response( R"({"ok":true})", outdata );
        case CMD_GET_MESSAGES:
            return write_response( debugger->message_data( target ), outdata );
        case CMD_POLL:
            {
            auto response = debugger->chart_data( target );
            if ( response.size() > 60000 )
                return write_response(
                    R"({"ok":false,"error":"Chart response is too large"})", outdata );
            response.pop_back();
            const auto budget = 65500 - response.size();
            response += R"(,"events":)" + debugger->message_data( target, budget ) + "}";
            // Keep the last point as the overlap anchor for client history.
            // Subsequent polls transfer only this anchor and new changes.
            for ( auto& item : target.expressions )
                while ( item.samples.size() > 1 ) item.samples.pop_front();
            return write_response( response, outdata );
            }
        case CMD_GET_RELOAD_OBJECTS:
            {
            std::string response = R"({"ok":true,"objects":[)";
            auto* manager = G_TECH_OBJECT_MNGR();
            bool first = true;
            for ( u_int i = 0; i < manager->get_count(); ++i )
                {
                const auto* object = manager->get_tech_objects( i );
                const auto id = object->get_serial_idx();
                if ( id == 0 || id > 999 ) continue;
                if ( !first ) response += ',';
                first = false;
                response += R"({"id":)" + std::to_string( id ) +
                    R"(,"name":)" + json_quote( object->get_name() ) +
                    R"(,"lua_name":)" + json_quote( object->get_name_in_Lua() ) +
                    R"(,"idle":)" + ( object->is_idle() ? "true}" : "false}" );
                if ( response.size() > MAX_RESPONSE_LENGTH - 1024 )
                    return write_response(
                        R"({"ok":false,"error":"Object list is too large"})", outdata );
                }
            return write_response( response + "]}", outdata );
            }
        case CMD_BROWSE_VARIABLES:
            return write_response(
                debugger->browse_variables( body ), outdata );
        case CMD_SET_VARIABLE:
            return write_response(
                debugger->set_variable( body ), outdata );
        case CMD_EXEC_CONTROLLER_COMMAND:
            {
            if ( body.empty() )
                return write_response(
                    R"({"ok":false,"error":"Invalid controller command id"})",
                    outdata );
            int command_id = -1;
            const auto parsed = std::from_chars(
                body.data(), body.data() + body.size(), command_id );
            if ( parsed.ec != std::errc{} ||
                parsed.ptr != body.data() + body.size() )
                return write_response(
                    R"({"ok":false,"error":"Invalid controller command id"})",
                    outdata );

            const auto controller_command =
                static_cast<PAC_info::COMMANDS>( command_id );
            const int reload_base = static_cast<int>(
                PAC_info::COMMANDS::RELOAD_TECH_OBJECT_BASE );
            if ( command_id > reload_base && command_id < reload_base + 1000 )
                {
                const auto id = command_id - reload_base;
                const int result = G_PAC_INFO()->set_cmd( "CMD", 0, command_id );
                const std::string message = result == 0 ?
                    "Object " + std::to_string( id ) + " reloaded." :
                    G_TECH_OBJECT_MNGR()->get_reload_error().substr( 0, 1024 );
                return write_response( std::string( R"({"ok":)" ) +
                    ( result == 0 ? "true" : "false" ) + R"(,"command":)" +
                    std::to_string( command_id ) + R"(,"object":)" +
                    std::to_string( id ) + R"(,"result":)" +
                    std::to_string( result ) + R"(,"queued":false,"message":)" +
                    json_quote( message ) + ( result == 0 ? "}" :
                        R"(,"error":)" + json_quote( message ) + "}" ), outdata );
                }
            switch ( controller_command )
                {
                case PAC_info::COMMANDS::CLEAR_RESULT_CMD:
                case PAC_info::COMMANDS::RELOAD_RESTRICTIONS:
                case PAC_info::COMMANDS::RESET_PARAMS:
                case PAC_info::COMMANDS::FORCE_SAVE_PARAMS:
                case PAC_info::COMMANDS::PHOENIX_MODBUS_UDP_ON:
                case PAC_info::COMMANDS::PHOENIX_MODBUS_UDP_OFF:
                    break;
                default:
                    return write_response(
                        R"({"ok":false,"error":"Unknown controller command"})",
                        outdata );
                }

            const int result = G_PAC_INFO()->set_cmd(
                "CMD", 0, static_cast<double>( command_id ) );
            const std::string response = std::string( R"({"ok":)" ) +
                ( result == 0 ? "true" : "false" ) +
                R"(,"command":)" + std::to_string( command_id ) +
                R"(,"result":)" + std::to_string( result ) +
                R"(,"queued":)" +
                ( controller_command == PAC_info::COMMANDS::FORCE_SAVE_PARAMS &&
                    result == 0 ? "true" : "false" ) +
                ( result == 0 ? "}" :
                    R"(,"error":"Controller command failed"})" );
            return write_response( response, outdata );
            }
        default:
            return write_response(
                R"({"ok":false,"error":"Unknown command"})", outdata );
        }
    }

#ifdef PTUSA_TEST
std::size_t lua_debugger::sessions_count() const
    {
    return sessions_.size();
    }

std::size_t lua_debugger::expressions_count(
    const std::string& session_id ) const
    {
    const auto found = sessions_.find( session_id );
    return found == sessions_.end() ? 0 : found->second.expressions.size();
    }

void lua_debugger::expire_sessions_for_test( std::uint32_t elapsed_ms )
    {
    const auto now = get_millisec();
    for ( auto& item : sessions_ )
        item.second.last_access_ms = now - elapsed_ms;
    expire_sessions();
    }
#endif
