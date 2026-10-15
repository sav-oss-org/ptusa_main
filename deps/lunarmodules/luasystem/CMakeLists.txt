set(SOURCES luasystem/src/bitflags.c
    luasystem/src/compat.c
    luasystem/src/core.c
    luasystem/src/environment.c
    luasystem/src/random.c
    luasystem/src/term.c
    luasystem/src/time.c
    luasystem/src/wcwidth.c)

# Create the library.
add_library(luasystem ${SOURCES})
target_include_directories(luasystem PRIVATE luasystem/src ${LUA_INCLUDE_DIR})
add_dependencies(luasystem liblua)
target_link_libraries(luasystem PRIVATE "$<TARGET_LINKER_FILE:liblua>")

set_target_properties(luasystem PROPERTIES
    OUTPUT_NAME core
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/system"
    LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/system"
    ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/system")

if (WIN32)
    target_compile_definitions(luasystem PRIVATE
    _CRT_NONSTDC_NO_WARNINGS _USRDLL _WINDOWS _WINDLL)

    target_link_libraries(luasystem PRIVATE advapi32 winmm bcrypt)
endif()
