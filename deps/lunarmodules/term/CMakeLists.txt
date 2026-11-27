set(SOURCES term/core.c)

# Create the library.
add_library(term_core ${SOURCES})

target_sources(term_core PRIVATE
    $<$<C_COMPILER_ID:MSVC>:term_core.def>)

include_directories(PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(term_core PRIVATE "$<TARGET_LINKER_FILE:liblua>")
add_dependencies(term_core liblua)

set_target_properties(term_core PROPERTIES
    OUTPUT_NAME core
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/term"
    LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/term"
    ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/term")
