cmake_minimum_required(VERSION 3.13)

# LuaSQL drivers to build (match the names of files ls_<driver>.c and <driver>.def)
set(LUASQL_DRIVERS "odbc" CACHE STRING "LuaSQL drivers to build (e.g. 'odbc;sqlite3;mysql;postgres;firebird')")

# Common path settings.
set(LUASQL_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/luasql/src")

add_compile_definitions(LUASQL_VERSION_NUMBER=\"2.7.1\")

# Helper function to add a driver.
function(add_luasql_driver driver)
    set(src_c "${LUASQL_SRC_DIR}/ls_${driver}.c")
    if(NOT EXISTS "${src_c}")
        message(WARNING "LuaSQL driver '${driver}' skipped: source not found: ${src_c}")
        return()
    endif()

    set(SOURCES
        "${LUASQL_SRC_DIR}/luasql.c"
        "${src_c}"
    )

    # Target name: luasql_<driver>, output file: <driver>.dll/.so without prefix.
    if(driver STREQUAL "odbc" AND NOT TARGET ODBC::ODBC)
        message(STATUS "ODBC runtime unavailable; skipping LuaSQL ODBC driver target")
        return()
    endif()

    add_library(luasql_${driver} ${SOURCES})

    if(MSVC)
        set_target_properties(luasql_${driver} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()

    target_include_directories(luasql_${driver} PRIVATE
        "${LUASQL_SRC_DIR}"
        ${LUA_INCLUDE_DIR}
    )
    target_link_libraries(luasql_${driver}
        PRIVATE "$<TARGET_LINKER_FILE:liblua>")
    add_dependencies(luasql_${driver} liblua)

    # Driver-specific dependencies.
    if(driver STREQUAL "odbc")
        find_package(ODBC QUIET)
        if(TARGET ODBC::ODBC)
            target_link_libraries(luasql_${driver} PRIVATE ODBC::ODBC)
            if(TARGET unixodbc_library_target)
                add_dependencies(luasql_${driver} unixodbc_library_target)
            endif()
        else()
            message(WARNING "ODBC not found; driver '${driver}' may fail to link")
        endif()

    elseif(driver STREQUAL "sqlite3")
        find_package(SQLite3 QUIET)
        if(TARGET SQLite::SQLite3)
            target_link_libraries(luasql_${driver} PRIVATE SQLite::SQLite3)
        else()
            find_library(SQLITE3_LIBRARY NAMES sqlite3)
            if(SQLITE3_LIBRARY)
                target_link_libraries(luasql_${driver} PRIVATE "${SQLITE3_LIBRARY}")
            else()
                message(WARNING "SQLite3 not found; driver '${driver}' may fail to link")
            endif()
        endif()
    elseif(driver STREQUAL "mysql")
        # Try to find MySQL/MariaDB client library.
        find_library(MYSQL_LIB NAMES libmysql mysqlclient mariadb)
        if(MYSQL_LIB)
            target_link_libraries(luasql_${driver} PRIVATE "${MYSQL_LIB}")
        else()
            message(WARNING "MySQL client library not found; driver '${driver}' may fail to link")
        endif()
    elseif(driver STREQUAL "postgres")
        find_package(PostgreSQL QUIET)
        if(TARGET PostgreSQL::PostgreSQL)
            target_link_libraries(luasql_${driver} PRIVATE PostgreSQL::PostgreSQL)
        else()
            message(WARNING "PostgreSQL not found; driver '${driver}' may fail to link")
        endif()
    elseif(driver STREQUAL "firebird")
        find_library(FIREBIRD_LIB NAMES fbclient firebird)
        if(FIREBIRD_LIB)
            target_link_libraries(luasql_${driver} PRIVATE "${FIREBIRD_LIB}")
        else()
            message(WARNING "Firebird client library not found; driver '${driver}' may fail to link")
        endif()
    endif()

    # MSVC options and defines, similar to Makefile.win.
    target_compile_definitions(luasql_${driver} PRIVATE
        $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS;WIN32>
    )

    # Output names and directories: luasql/<driver>.dll
    set_target_properties(luasql_${driver} PROPERTIES
        OUTPUT_NAME "${driver}"
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/luasql"
        LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/luasql"
        ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/luasql"
    )
endfunction()

# Adding declared drivers.
foreach(d ${LUASQL_DRIVERS})
    string(TOLOWER "${d}" d_lower)
    add_luasql_driver("${d_lower}")
endforeach()
