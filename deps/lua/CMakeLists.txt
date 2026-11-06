if(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0")
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
endif()

# Build Lua 5.5 from official source (files are in the root, not src/).
set(LUA_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lua")

set(SRC_LIB
    ${LUA_SRC_DIR}/lapi.c
    ${LUA_SRC_DIR}/lauxlib.c
    ${LUA_SRC_DIR}/lbaselib.c
    ${LUA_SRC_DIR}/lcode.c
    ${LUA_SRC_DIR}/lcorolib.c
    ${LUA_SRC_DIR}/lctype.c
    ${LUA_SRC_DIR}/ldblib.c
    ${LUA_SRC_DIR}/ldebug.c
    ${LUA_SRC_DIR}/ldo.c
    ${LUA_SRC_DIR}/ldump.c
    ${LUA_SRC_DIR}/lfunc.c
    ${LUA_SRC_DIR}/lgc.c
    ${LUA_SRC_DIR}/linit.c
    ${LUA_SRC_DIR}/liolib.c
    ${LUA_SRC_DIR}/llex.c
    ${LUA_SRC_DIR}/lmathlib.c
    ${LUA_SRC_DIR}/lmem.c
    ${LUA_SRC_DIR}/loadlib.c
    ${LUA_SRC_DIR}/lobject.c
    ${LUA_SRC_DIR}/lopcodes.c
    ${LUA_SRC_DIR}/loslib.c
    ${LUA_SRC_DIR}/lparser.c
    ${LUA_SRC_DIR}/lstate.c
    ${LUA_SRC_DIR}/lstring.c
    ${LUA_SRC_DIR}/lstrlib.c
    ${LUA_SRC_DIR}/ltable.c
    ${LUA_SRC_DIR}/ltablib.c
    ${LUA_SRC_DIR}/ltm.c
    ${LUA_SRC_DIR}/lundump.c
    ${LUA_SRC_DIR}/lutf8lib.c
    ${LUA_SRC_DIR}/lvm.c
    ${LUA_SRC_DIR}/lzio.c
)

set(SRC_LUA ${LUA_SRC_DIR}/lua.c)

# Platform-specific settings.
set(LUA_LIBS)
if(UNIX)
    find_library(MATH_LIBRARY m)
    if(MATH_LIBRARY)
        list(APPEND LUA_LIBS ${MATH_LIBRARY})
    endif()
    find_library(DL_LIBRARY dl)
    if(DL_LIBRARY)
        list(APPEND LUA_LIBS ${DL_LIBRARY})
    endif()
    add_compile_definitions(LUA_USE_POSIX LUA_USE_DLOPEN)
endif()

include_directories(${LUA_SRC_DIR})

# Shared library.
add_library(liblua SHARED ${SRC_LIB})
target_link_libraries(liblua ${LUA_LIBS})
set_target_properties(liblua PROPERTIES
    OUTPUT_NAME lua
    CLEAN_DIRECT_OUTPUT 1)
if(WIN32)
    set_target_properties(liblua PROPERTIES COMPILE_DEFINITIONS LUA_BUILD_AS_DLL)
endif()

# Static library.
add_library(liblua_static STATIC ${SRC_LIB})
target_link_libraries(liblua_static ${LUA_LIBS})

# Interpreter executable.
add_executable(lua ${SRC_LUA})
target_link_libraries(lua liblua)
