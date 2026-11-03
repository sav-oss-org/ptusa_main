#include "smart_ptr.h"

#ifdef  __cplusplus
extern "C" {
#endif

#include    "lua.h"
#include    "lauxlib.h"
#include    "lualib.h"

#ifdef  __cplusplus
    };
#endif

#include    "tolua++.h"
//-----------------------------------------------------------------------------
TOLUA_API int tolua_PAC_dev_open ( lua_State* tolua_S );
//-----------------------------------------------------------------------------
const int SYS_FILE_CNT = 3;
const int FILE_CNT     = 7;
/// Index of `main.restrictions.lua` in FILES[]. Keep it last so the
/// restrictions hot reload survives adding or removing earlier scripts.
const int RESTRICTIONS_SCRIPT_N = FILE_CNT - 1;
//-----------------------------------------------------------------------------
extern const char *FILES[ FILE_CNT ];
//-----------------------------------------------------------------------------
const int FILES_VERSION[ FILE_CNT ] =
    {
    2, //"sys.io.lua",
    2, //"sys.devices.lua",
    9, //"sys.objects.plua",

    1, //"main.io.plua",
    1, //"main.objects.plua",
    1, //"main.modbus_srv.lua",
    1, //"main.restrictions.lua"
    };
//-----------------------------------------------------------------------------
int check_file(const char* file_name, char* err_str);
//-----------------------------------------------------------------------------
class lua_manager
    {
    public:
        static void no_print_stack_traceback()
            {
            is_print_stack_traceback = false;
            }

        static void use_print_stack_traceback()
            {
            is_print_stack_traceback = true;
            }

        static lua_manager* get_instance();

        int init( lua_State* L, const char* script_name, const char* dir = "",
            const char* sys_dir = "", const char* extra_dirs = "" );

        ~lua_manager();

        bool is_exist_lua_function( const char* object_name,
            const char* function_name ) const;

        int void_exec_lua_method( const char *object_name,
            const char *function_name, const char *c_function_name ) const;

        const char* char_no_param_exec_lua_method( const char *object_name,
            const char *function_name, const char *c_function_name ) const;

        const char* char_exec_lua_method( const char *object_name,
            const char *function_name, int param, const char *c_function_name ) const;

        int int_exec_lua_method( const char *object_name,
            const char *function_name, int param,
            const char *c_function_name ) const;

        int int_2_exec_lua_method( const char* object_name,
            const char* function_name, int param1, int param2,
            const char* c_function_name ) const;

        int int_no_param_exec_lua_method( const char *object_name,
            const char *function_name, const char *c_function_name ) const;

        void* user_object_exec_lua_method( const char *object_name,
            const char *function_name, int param,
            const char *c_function_name ) const;

        int exec_Lua_str( const char *Lua_str, const char *error_str,
            bool is_print_error_msg = true ) const;

        lua_State * get_Lua() const;

        int reload_script( int script_n, const char* script_function_name,
            char *res_str, int max_res_str_length );

#ifdef PTUSA_TEST
        void set_Lua( lua_State* l );

        void free_Lua();
#endif

    private:
        lua_manager() : L( 0 ), err_func( 0 ), is_free_lua( 0 )
            {
            }

        static int error_trace( lua_State * L );

        static auto_smart_ptr< lua_manager > instance;

        int exec_lua_method( const char *object_name,
            const char *function_name, int param, int is_use_param = 1,
            int is_use_lua_return_value = 1 ) const;

        int exec_lua_method_var( const char* object_name, const char* function_name,
            int is_use_lua_return_value = 0, int cnt = 0, ... ) const;

        static bool is_print_stack_traceback;

        int err_func;
        lua_State * L;

        int is_free_lua;

        enum CONSTANTS
            {
            MAX_ERRORS = 50,
            };
    };
//-----------------------------------------------------------------------------
#define G_LUA_MANAGER lua_manager::get_instance()
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
