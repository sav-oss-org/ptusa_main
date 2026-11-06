#include "../includes.h"

#ifdef  __cplusplus
extern "C" {
#endif

#include    "lua.h"
#include    "lauxlib.h"
#include    "lualib.h"

#ifdef  __cplusplus
    };
#endif

using namespace ::testing;

extern int G_DEBUG;
extern "C" int luaopen_ptusa_main( lua_State* L );
extern int no_print_stack_traceback( lua_State* L );
extern int use_print_stack_traceback( lua_State* L );
extern int switch_on_verbose( lua_State* L );
extern int switch_off_verbose( lua_State* L );
extern int lua_init( lua_State* L );


TEST( dll, luaopen_ptusa_main )
    {
    auto L = luaL_newstate();
    auto res = luaopen_ptusa_main( L );
    EXPECT_EQ( 1, res );

    // Validate that a module table is on top of the stack.
    EXPECT_EQ( 1, lua_gettop( L ) );
    EXPECT_TRUE( lua_istable( L, -1 ) );
    lua_close( L );
    }

TEST( dll, no_print_stack_traceback )
    {
    EXPECT_EQ( 0, no_print_stack_traceback( nullptr ) );
    }

TEST( dll, use_print_stack_traceback )
    {
    EXPECT_EQ( 0, use_print_stack_traceback( nullptr ) );
    }

TEST( dll, switch_on_verbose )
    {
    G_DEBUG = 0;
    switch_on_verbose( nullptr );
    EXPECT_EQ( 1, G_DEBUG );
    }

TEST( dll, switch_off_verbose )
    {
    G_DEBUG = 1;
    switch_off_verbose( nullptr );
    EXPECT_EQ( 0, G_DEBUG );
    G_DEBUG = 1;
    }

TEST( dll, lua_init )
    {
    auto L = luaL_newstate();

    auto res = lua_init( L );
    luaL_openlibs( L );
    EXPECT_EQ( 1, res );

    char argv0[] = "ptusa_main.exe";
    char argv1[] = "main.plua";
    char argv2[] = "--sys_path";
    char argv3[] = "./sys/";
    char argv4[] = "--path";
    char argv5[] = "./";

    lua_pushlstring( L, argv0, sizeof( argv0 ) );
    lua_pushlstring( L, argv1, sizeof( argv1 ) );
    lua_pushlstring( L, argv2, sizeof( argv2 ) );
    lua_pushlstring( L, argv3, sizeof( argv3 ) );
    lua_pushlstring( L, argv4, sizeof( argv4 ) );
    lua_pushlstring( L, argv5, sizeof( argv5 ) );

    // Функция инициализации возвращает количество результатов, помещенных
    // в стек Lua.
    res = lua_init( L );
    EXPECT_EQ( 1, res );

    // Получаем из стека результат выполнения инициализации. Это число, в
    // данном случае будет ошибка.
    res = lua_gettop( L );
    EXPECT_EQ( 1, res );
    auto t = lua_type( L, 1 );
    EXPECT_EQ( LUA_TNUMBER, t );
    res = static_cast<int>( lua_tonumber( L, 1 ) );
    EXPECT_EQ( res, EXIT_FAILURE );
    lua_remove( L, 1 );

    lua_close( L );
    L = nullptr;
    }
