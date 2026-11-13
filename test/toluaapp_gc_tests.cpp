#include "gtest/gtest.h"

#include <cstdlib>

// Lua headers have no extern "C" guards (see common/lua_manager.h).
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "tolua++.h"
}

/*
    Regression test for the Lua 5.5 upgrade (#1277).

    toluapp attaches its class metatables (whose __gc handler is
    class_gc_event) to the class metatable itself via mapinheritance.
    Lua 5.4+ also finalizes tables that have a __gc metamethod, and
    class_gc_event dereferenced the NULL result of lua_touserdata() for
    such tables.  Without the guarded __gc registration of
    deps/toluapp/lua51compat.h this test crashes.
*/

TEST(toluaapp_gc_test, class_metatable_finalization_does_not_crash)
{
    lua_State* L = luaL_newstate();
    ASSERT_NE(nullptr, L);
    luaL_openlibs(L);

    tolua_open(L);
    tolua_module(L, NULL, 0);
    tolua_beginmodule(L, NULL);

    // Class registration (as done by generated .pkg bindings) turns the
    // class metatable into a finalizable table.
    tolua_usertype(L, "GcSmoke");
    tolua_cclass(L, "gcsmoke", "GcSmoke", "", NULL);

    void* obj = malloc(64);
    ASSERT_NE(nullptr, obj);
    // Owned instance: exercises the userdata path of the same __gc handler.
    tolua_pushusertype_and_takeownership(L, obj, "GcSmoke");
    lua_setglobal(L, "gcsmoke");

    // A full collection finalizes the class metatable through the guarded
    // __gc handler.
    lua_gc(L, LUA_GCCOLLECT, 0);

    // lua_close runs the remaining finalizers and releases the instance
    // (the default collector frees it).
    lua_close(L);
}
