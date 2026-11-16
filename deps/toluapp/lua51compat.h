/* lua51compat.h
** Lua 5.1 compatibility shim for building toluapp with Lua 5.2+.
** Force-included before each toluapp translation unit via -include.
*/

#ifndef LUA_51_COMPAT_H
#define LUA_51_COMPAT_H

/* Include Lua headers first so LUA_VERSION_NUM is available. */
#include "lua.h"
#include "lauxlib.h"

#if LUA_VERSION_NUM >= 502

/* luaL_getn() was removed in Lua 5.2; use lua_rawlen() instead. */
#ifndef luaL_getn
#define luaL_getn(L, i) ((int)(lua_rawlen)(L, i))
#endif

/* lua_getfenv()/lua_setfenv() were removed in Lua 5.2.
** Full userdata now stores its associated value via user values. */
static inline void lua_getfenv(lua_State* L, int idx)
    {
    lua_getuservalue(L, idx);
    }

static inline int lua_setfenv(lua_State* L, int idx)
    {
    lua_setuservalue(L, idx);
    return 1;
    }

/* LUA_GLOBALSINDEX was removed in Lua 5.2.
** toluapp only uses it as an argument of lua_pushvalue(), so override that
** call shape and leave every other index untouched. */
#ifndef LUA_GLOBALSINDEX
#define LUA_GLOBALSINDEX (-10001)

/* Rename the real function so the macro can call it without recursion. */
#define _lua_pushvalue_real(L, idx) (lua_pushvalue)(L, idx)

#define lua_pushvalue(L, idx) \
    do { \
        if ((idx) == LUA_GLOBALSINDEX) \
            lua_pushglobaltable(L); \
        else \
            _lua_pushvalue_real(L, idx); \
    } while (0)
#endif /* LUA_GLOBALSINDEX */

/* Lua 5.4+ also runs __gc finalizers for tables whose metatable has one.
** toluapp registers class_gc_event (which dereferences the result of
** lua_touserdata(L, 1)) as the __gc handler of its class metatables, so it
** is called with the class table itself when that table is collected.
** lua_touserdata() returns NULL for a table and the unchecked dereference
** of that NULL crashes.
**
** Redefining lua_touserdata globally would break the NULL checks that
** toluapp relies on as validity tests, so intercept only the registration
** of the collector closure: when lua_pushcclosure() is asked to wrap
** exactly class_gc_event, wrap a guarded copy instead that ignores every
** value that is not full userdata.  The guard is created as a closure with
** the same upvalues (the "tolua_gc" and "tolua_super" registry tables), so
** class_gc_event keeps working unchanged for real userdata objects. */

/* class_gc_event is declared later (in tolua++.h); declare it here so the
** guard below compiles at force-include time. */
extern int class_gc_event (lua_State* L);

static int _lua51compat_class_gc(lua_State* L)
    {
    if (!lua_isuserdata(L, 1))
        {
        return 0; /* Table finalization: nothing for toluapp to collect. */
        }
    return class_gc_event(L);
    }

#define _lua51compat_pushcclosure(L, f, n) \
    do { \
        lua_CFunction _lua51compat_f = (f); \
        if (_lua51compat_f == class_gc_event) \
            _lua51compat_f = _lua51compat_class_gc; \
        (lua_pushcclosure)((L), _lua51compat_f, (n)); \
    } while (0)

#define lua_pushcclosure(L, f, n) _lua51compat_pushcclosure(L, f, n)

#endif /* LUA_VERSION_NUM >= 502 */

#endif /* LUA_51_COMPAT_H */
