#include "lua_manager_dependencies.h"

void LuaManagerTest::SetUp()
{
    ASSERT_EQ( G_LUA_MANAGER->get_Lua(), nullptr );

	lua_hooks.push_back(subhook_new((void *) lua_pushcclosure,      (void *) mock_lua_pushcclosure,     SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) lua_gettop,            (void *) mock_lua_gettop,           SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) lua_type,				(void *) mock_lua_type,				SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) lua_getfield,          (void *) mock_lua_getfield,         SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) lua_getglobal,         (void *) mock_lua_getglobal,        SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) lua_rotate,            (void *) mock_lua_rotate,           SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) lua_pcallk,            (void *) mock_lua_pcallk,           SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) lua_pushnumber,        (void *) mock_lua_pushnumber,       SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) tolua_tostring,        (void *) mock_tolua_tostring,       SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) tolua_tonumber,        (void *) mock_tolua_tonumber,       SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) luaL_loadstring,       (void *) mock_luaL_loadstring,      SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) tolua_tousertype,      (void *) mock_tolua_tousertype,     SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) luaL_loadfilex,        (void *) mock_luaL_loadfilex,       SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) luaL_newstate,         (void *) mock_luaL_newstate,        SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) lua_gc,                (void *) mock_lua_gc,               SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) luaL_openselectedlibs, (void *) mock_luaL_openselectedlibs,SUBHOOK_64BIT_OFFSET));
	lua_hooks.push_back(subhook_new((void *) tolua_PAC_dev_open,    (void *) mock_tolua_PAC_dev_open,   SUBHOOK_64BIT_OFFSET));

	lua_hooks.push_back(subhook_new((void *) check_file,            (void *) mock_check_file,           SUBHOOK_64BIT_OFFSET));
    lua_hooks.push_back(subhook_new((void *) G_TECH_OBJECT_MNGR,    (void *) mock_G_TECH_OBJECT_MNGR,   SUBHOOK_64BIT_OFFSET));
    lua_hooks.push_back(subhook_new((void *) lua_tolstring,         (void *) mock_lua_tolstring,        SUBHOOK_64BIT_OFFSET));
    lua_hooks.push_back(subhook_new((void *) lua_settop,            (void *) mock_lua_settop,           SUBHOOK_64BIT_OFFSET));

    lua_hooks.push_back(subhook_new((void*) &lua_close,             (void*) &mock_lua_close,             SUBHOOK_64BIT_OFFSET));

    // Install hooks
    for ( auto hook : lua_hooks )
        {
        subhook_install( hook );
        }
    }

void LuaManagerTest::TearDown()
    {
    // Remove the hooks and free memory.
    for ( auto hook : lua_hooks )
        {
        subhook_remove( hook );
        subhook_free( hook );
        }

    if ( need_free_Lua_state )
        {
        // Так как Lua создавали с помощью new lua_state в mock_luaL_newstate
        // (или сразу с помощью new lua_state),
        // то удаляем с помощью delete.
        delete G_LUA_MANAGER->get_Lua();
        G_LUA_MANAGER->set_Lua( nullptr );
        need_free_Lua_state = false;
        }

	ASSERT_EQ( G_LUA_MANAGER->get_Lua(), nullptr );
}

void set_file_counter(int val)
{
    file_counter = val;
}

void set_lua_pcall_success_calls_before_failure(int val)
{
    lua_pcall_state = val;
}

/****************** MOCKED (HOOKED) FUNCTIONS *****************/

void mock_lua_pushcclosure(lua_State *L, lua_CFunction fn, int n)
{}

int mock_lua_gettop(lua_State *L)
{
	return 0;
}

int mock_lua_type(lua_State* L, int idx)
{
	return -1;
}

void mock_lua_getfield(lua_State *L, int idx, const char *k)
{}

int mock_lua_getglobal(lua_State *L, const char *name)
{
    return 0; // LUA_TNIL.
}

/* lua_remove(L, idx) is now a macro: (lua_rotate(L, idx, -1), lua_pop(L, 1)).
** Hook lua_rotate as a no-op; lua_pop/lua_settop is hooked separately. */
void mock_lua_rotate(lua_State *L, int idx, int n)
{}

/* lua_pcall(L,n,r,f) is now a macro calling lua_pcallk(L,n,r,f,0,NULL). */
int mock_lua_pcallk(lua_State *L, int nargs, int nresults, int errfunc,
    lua_KContext ctx, lua_KFunction k)
{
	return 0;
}

int mock_luaL_loadstring(lua_State *L, const char *s, bool is_print_error_msg)
{
	return 0;
}

void mock_lua_pushnumber(lua_State *L, lua_Number n)
{}

const char* mock_tolua_tostring(lua_State* L, int narg, const char* def)
{
	return "Test passed";
}

lua_Number mock_tolua_tonumber(lua_State* L, int narg, lua_Number def)
{
	return 0.0;
}

void* mock_tolua_tousertype(lua_State* L, int narg, void* def)
{
	return (void*) "Test passed";
}

/* luaL_loadfile(L,f) is now a macro calling luaL_loadfilex(L,f,NULL). */
int mock_luaL_loadfilex(lua_State *L, const char *filename, const char *mode)
{
	return 0;
}

lua_State* mock_luaL_newstate(void)
{
    return new lua_State{};
}

int	mock_lua_gc(lua_State *L, int what, int data)
{
	return 0;
}

/* luaL_openlibs(L) is now a macro calling luaL_openselectedlibs(L, ~0, 0). */
void mock_luaL_openselectedlibs(lua_State *L, int load, int preload)
{}

int	mock_tolua_PAC_dev_open(lua_State* tolua_S)
{
	return 0;
}

int	mock_tolua_IOT_dev_open(lua_State* tolua_S)
{
	return 0;
}

int	mock_tolua_OPC_UA_open(lua_State* tolua_S)
{
	return 0;
}

void mock_lua_close(lua_State * L)
{}

int mock_check_file(const char * file_name, char * err_str)
{
    int res = FILES_VERSION[ file_counter++ ];
    if ( file_counter == FILE_CNT ) {
        file_counter = 0;
    }
    return res;
}

tech_object_manager * mock_G_TECH_OBJECT_MNGR()
{
	return mock_tech_object_manager::get_instance();
}

lua_State * mock_luaL_newstate_failure(void)
{
    return NULL;
}

/* Special failure mock for luaL_loadfilex (replaces luaL_loadfile). */
int mock_luaL_loadfilex_failure(lua_State * L, const char * filename,
    const char * mode)
{
    return 1;
}

int mock_luaL_loadfilex_failure_2(lua_State * L, const char * filename,
    const char * mode)
{
    // return 0 (success) when called less then FILE_CNT times, then fail.
    return (file_counter++ < FILE_CNT ? 0 : 1);
}

const char * mock_lua_tolstring(lua_State * L, int idx, size_t * len)
{
    return "lua_tolstring hook called";
}

void mock_lua_settop(lua_State * L, int idx)
{}

int mock_check_file_failure(const char * file_name, char * err_str)
{
    strcpy(err_str, "mock_check_file_failure called");
    return -1;
}

int mock_check_file_failure_2(const char * file_name, char * err_str)
{
    return INT_MAX;
}

/* Special failure mock for lua_pcallk (replaces lua_pcall). */
int mock_lua_pcallk_failure(lua_State * L, int nargs, int nresults, int errfunc,
    lua_KContext ctx, lua_KFunction k)
{
    return ( --lua_pcall_state < 0 ? 1 : 0 );
}
