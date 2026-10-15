cmake_minimum_required(VERSION 3.31)
project(lmdb C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(LMDB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/lmdb/libraries/liblmdb)
set(LMDB_SOURCES
    ${LMDB_ROOT}/mdb.c
    ${LMDB_ROOT}/midl.c
)

if(MINGW)
    add_compile_options(-Wno-error=incompatible-pointer-types)
endif()

option(LMDB_ENABLE_CRYPTO "Build LMDB crypto support with libsodium" OFF)

if (LMDB_ENABLE_CRYPTO)
    find_path(SODIUM_INCLUDE_DIR sodium.h
        HINTS
            $ENV{SODIUM_ROOT}/include
            $ENV{LIBSODIUM_ROOT}/include
            /usr/include
            /usr/local/include
            $ENV{ProgramFiles}/libsodium/include
            $ENV{ProgramFiles(x86)}/libsodium/include
    )

    find_library(SODIUM_LIBRARY NAMES sodium libsodium
        HINTS
            $ENV{SODIUM_ROOT}/lib
            $ENV{LIBSODIUM_ROOT}/lib
            /usr/lib
            /usr/local/lib
            $ENV{ProgramFiles}/libsodium/lib
            $ENV{ProgramFiles(x86)}/libsodium/lib
    )

    if (NOT SODIUM_INCLUDE_DIR OR NOT SODIUM_LIBRARY)
        message(FATAL_ERROR "libsodium not found. LMDB crypto requires sodium.h and sodium library.\n"
            "Set SODIUM_INCLUDE_DIR and SODIUM_LIBRARY or install libsodium.")
    endif()

    list(APPEND LMDB_SOURCES
        ${LMDB_ROOT}/crypto.c
        ${LMDB_ROOT}/cryptoc.c
    )
endif()

add_library(lmdb STATIC ${LMDB_SOURCES})

target_include_directories(lmdb PUBLIC ${LMDB_ROOT})

if (LMDB_ENABLE_CRYPTO)
    target_include_directories(lmdb PUBLIC ${SODIUM_INCLUDE_DIR})
    target_link_libraries(lmdb PUBLIC ${SODIUM_LIBRARY})
endif()

if (MSVC)
    target_compile_definitions(lmdb PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
