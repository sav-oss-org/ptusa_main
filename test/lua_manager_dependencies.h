#pragma once
#include "includes.h"
#include "lua_manager.h"
#include "lauxlib.h"
#include "lua.h"
#include "lstate.h"
#include "mock_params_manager.h"
#include "mock_project_manager.h"
#include "mock_tech_object_manager.h"
#include "mock_tcp_communicator.h"

using namespace std;

class LuaManagerTest : public ::testing::Test
{
protected:
	vector<subhook_t>   lua_hooks;
	virtual void        SetUp();
	virtual void        TearDown();

    bool need_free_Lua_state = false;
};

static int file_counter = 0;
static int lua_pcall_state = 0;
void set_file_counter(int val);
void set_lua_pcall_success_calls_before_failure(int val);

// general mocks of hooked functions
void        mock_lua_pushcclosure(lua_State *L, lua_CFunction fn, int n);
int         mock_lua_gettop(lua_State *L);
int         mock_lua_type( lua_State* L, int idx );
void        mock_lua_getfield(lua_State *L, int idx, const char *k);
int         mock_lua_getglobal(lua_State *L, const char *name);
void        mock_lua_rotate(lua_State *L, int idx, int n);
int         mock_lua_pcallk(lua_State *L, int nargs, int nresults, int errfunc,
                lua_KContext ctx, lua_KFunction k);
int         mock_luaL_loadstring(lua_State *L, const char *s, bool is_print_error_msg = true);
void        mock_lua_pushnumber(lua_State *L, lua_Number n);
const char* mock_tolua_tostring(lua_State* L, int narg, const char* def);
lua_Number  mock_tolua_tonumber(lua_State* L, int narg, lua_Number def);
void*       mock_tolua_tousertype(lua_State* L, int narg, void* def);
int         mock_luaL_loadfilex(lua_State *L, const char *filename, const char *mode);
lua_State*  mock_luaL_newstate (void);
int         mock_lua_gc(lua_State *L, int what, int data);
void        mock_luaL_openselectedlibs(lua_State *L, int load, int preload);
int         mock_tolua_PAC_dev_open(lua_State* tolua_S);
int         mock_tolua_IOT_dev_open(lua_State* tolua_S);
int         mock_tolua_OPC_UA_open(lua_State* tolua_S);
void        mock_lua_close(lua_State *L);
int         mock_check_file(const char* file_name, char* err_str);
tech_object_manager* mock_G_TECH_OBJECT_MNGR();
const char* mock_lua_tolstring(lua_State *L, int idx, size_t *len);
void        mock_lua_settop(lua_State *L, int idx);

// special mocks of hooked functions
lua_State*  mock_luaL_newstate_failure(void);
int         mock_luaL_loadfilex_failure(lua_State *L, const char *filename,
                const char *mode);
int         mock_luaL_loadfilex_failure_2(lua_State *L, const char *filename,
                const char *mode);
int         mock_check_file_failure(const char* file_name, char* err_str);
int         mock_check_file_failure_2(const char* file_name, char* err_str);
int         mock_lua_pcallk_failure(lua_State *L, int nargs, int nresults,
                int errfunc, lua_KContext ctx, lua_KFunction k);