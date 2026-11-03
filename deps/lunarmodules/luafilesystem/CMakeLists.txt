set(SOURCES luafilesystem/src/lfs.c)

add_library(lfs ${SOURCES})

# Используем .def только под MSVC (Visual Studio).
target_sources(lfs PRIVATE $<$<C_COMPILER_ID:MSVC>:luafilesystem/src/lfs.def>)

target_include_directories(lfs PRIVATE luafilesystem/src src/lib ${LUA_INCLUDE_DIR})
add_dependencies(lfs liblua)
target_link_libraries(lfs PRIVATE "$<TARGET_LINKER_FILE:liblua>")

target_compile_definitions(lfs PRIVATE $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)

set_target_properties (lfs PROPERTIES PREFIX "")
