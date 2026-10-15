cmake_minimum_required(VERSION 3.13)

project(unixodbc LANGUAGES C)

# Native Windows builds use the system ODBC libraries.
# AXCF cross-builds are still hosted on Windows, but CMAKE_CROSSCOMPILING is true,
# so they must not take the native Windows branch. For cross builds, prefer the
# SDK/sysroot ODBC library and only fall back to external UnixODBC if it is not
# present in the toolchain root.
if(WIN32 AND NOT CMAKE_CROSSCOMPILING)
    find_package(ODBC REQUIRED)
    if(NOT TARGET ODBC::ODBC)
        add_library(ODBC::ODBC UNKNOWN IMPORTED GLOBAL)
        set_target_properties(ODBC::ODBC PROPERTIES
            IMPORTED_LOCATION "${ODBC_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${ODBC_INCLUDE_DIR}"
        )
    endif()
    return()
endif()

if(TARGET ODBC::ODBC)
    return()
endif()

# Prefer system ODBC from the active toolchain/sysroot for cross builds.
if(CMAKE_CROSSCOMPILING)
    set(_odbc_sysroot_hints)
    if(CMAKE_SYSROOT)
        list(APPEND _odbc_sysroot_hints "${CMAKE_SYSROOT}")
    endif()
    if(CMAKE_FIND_ROOT_PATH)
        list(APPEND _odbc_sysroot_hints ${CMAKE_FIND_ROOT_PATH})
    endif()

    find_library(ODBC_LIBRARY
        NAMES odbc libodbc
        HINTS ${_odbc_sysroot_hints}
        PATH_SUFFIXES lib usr/lib
        NO_DEFAULT_PATH
    )

    if(ODBC_LIBRARY)
        find_path(ODBC_INCLUDE_DIR
            NAMES sql.h sqlext.h
            HINTS ${_odbc_sysroot_hints}
            PATH_SUFFIXES include usr/include
            NO_DEFAULT_PATH
        )
    endif()
endif()

if(ODBC_LIBRARY AND ODBC_INCLUDE_DIR)
    if(NOT TARGET ODBC::ODBC)
        add_library(ODBC::ODBC UNKNOWN IMPORTED GLOBAL)
        set_target_properties(ODBC::ODBC PROPERTIES
            IMPORTED_LOCATION "${ODBC_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${ODBC_INCLUDE_DIR}"
        )
    endif()
    message(STATUS "Using toolchain sysroot ODBC library: ${ODBC_LIBRARY}")
    return()
endif()

find_package(ODBC QUIET)
if(ODBC_FOUND)
    if(NOT TARGET ODBC::ODBC)
        add_library(ODBC::ODBC UNKNOWN IMPORTED GLOBAL)
        set_target_properties(ODBC::ODBC PROPERTIES
            IMPORTED_LOCATION "${ODBC_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${ODBC_INCLUDE_DIR}"
        )
    endif()
    message(STATUS "Using system ODBC library: ${ODBC_LIBRARY}")
    return()
endif()

include(ExternalProject)

if(CMAKE_CROSSCOMPILING)
    set(_unixodbc_host_triplet "")
    get_filename_component(_unixodbc_compiler_name "${CMAKE_C_COMPILER}" NAME_WE)
    if(_unixodbc_compiler_name MATCHES "^(.*)-gcc$")
        set(_unixodbc_host_triplet "${CMAKE_MATCH_1}")
    elseif(_unixodbc_compiler_name MATCHES "^(.*)-g\+\+$")
        set(_unixodbc_host_triplet "${CMAKE_MATCH_1}")
    endif()

    if(NOT _unixodbc_host_triplet)
        get_filename_component(_unixodbc_compiler_dir "${CMAKE_C_COMPILER}" DIRECTORY)
        get_filename_component(_unixodbc_compiler_parent "${_unixodbc_compiler_dir}" NAME)
        if(_unixodbc_compiler_parent MATCHES "^([A-Za-z0-9_]+-pxc-linux(?:-[A-Za-z0-9_]+)?)$")
            set(_unixodbc_host_triplet "${CMAKE_MATCH_1}")
        endif()
    endif()

    if(NOT _unixodbc_host_triplet)
        set(_unixodbc_host_triplet "${CMAKE_SYSTEM_PROCESSOR}")
    endif()

    set(_unixodbc_cppflags "${CMAKE_C_FLAGS_INIT} ${CMAKE_CXX_FLAGS_INIT}")
    set(_unixodbc_ldflags "${CMAKE_EXE_LINKER_FLAGS_INIT} ${CMAKE_SHARED_LINKER_FLAGS_INIT}")
    if(CMAKE_SYSROOT)
        string(APPEND _unixodbc_cppflags " --sysroot=${CMAKE_SYSROOT}")
        string(APPEND _unixodbc_ldflags " --sysroot=${CMAKE_SYSROOT}")
    endif()

    foreach(_unixodbc_root IN LISTS CMAKE_FIND_ROOT_PATH)
        string(APPEND _unixodbc_cppflags " -I${_unixodbc_root}/usr/include")
    endforeach()

    message(STATUS
        "No usable ODBC library found in the active toolchain/sysroot; "
        "attempting cross-built UnixODBC with target compiler '${CMAKE_C_COMPILER}' "
        "and host '${_unixodbc_host_triplet}'"
    )
else()
    message(STATUS "No ODBC library found in active toolchain/sysroot; falling back to cross-built UnixODBC")
endif()

set(UNIXODBC_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/unixODBC")
set(UNIXODBC_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/unixodbc-build")
set(UNIXODBC_INSTALL_DIR "${CMAKE_BINARY_DIR}/unixodbc-install")

find_program(POSIX_SHELL_EXECUTABLE NAMES sh bash REQUIRED)

function(_to_bash_path out_var in_path)
    set(_path "${in_path}")
    string(REPLACE "\\" "/" _path "${_path}")
    if(_path MATCHES "^([A-Za-z]):/(.*)$")
        string(TOLOWER "${CMAKE_MATCH_1}" _drive)
        set(_path "/mnt/${_drive}/${CMAKE_MATCH_2}")
    elseif(_path MATCHES "^/([A-Za-z])/(.*)$")
        string(TOLOWER "${CMAKE_MATCH_1}" _drive)
        set(_path "/mnt/${_drive}/${CMAKE_MATCH_2}")
    endif()
    set(${out_var} "${_path}" PARENT_SCOPE)
endfunction()

_to_bash_path(UNIXODBC_SOURCE_DIR_BASH "${UNIXODBC_SOURCE_DIR}")
_to_bash_path(UNIXODBC_BUILD_DIR_BASH "${UNIXODBC_BUILD_DIR}")
_to_bash_path(UNIXODBC_INSTALL_DIR_BASH "${UNIXODBC_INSTALL_DIR}")
_to_bash_path(UNIXODBC_C_COMPILER_BASH "${CMAKE_C_COMPILER}")
_to_bash_path(UNIXODBC_CXX_COMPILER_BASH "${CMAKE_CXX_COMPILER}")
if(CMAKE_SYSROOT)
    _to_bash_path(UNIXODBC_SYSROOT_BASH "${CMAKE_SYSROOT}")
else()
    set(UNIXODBC_SYSROOT_BASH "")
endif()

string(STRIP "${_unixodbc_cppflags}" _unixodbc_cppflags)
string(STRIP "${_unixodbc_ldflags}" _unixodbc_ldflags)

if(UNIXODBC_SYSROOT_BASH)
    string(PREPEND _unixodbc_cppflags "--sysroot=${UNIXODBC_SYSROOT_BASH} ")
    string(PREPEND _unixodbc_ldflags "--sysroot=${UNIXODBC_SYSROOT_BASH} ")
endif()

set(UNIXODBC_CONFIGURE_ARGS
    --prefix=${UNIXODBC_INSTALL_DIR_BASH}
    --enable-shared=yes
    --enable-static=no
    --disable-gui
    --disable-debug
    --enable-driver-config=no
    --enable-drivers=no
    --without-odbcini
    --without-odbcinstini
)
if(CMAKE_CROSSCOMPILING)
    list(APPEND UNIXODBC_CONFIGURE_ARGS --host=${_unixodbc_host_triplet})

    set(_unixodbc_build_triplet "${CMAKE_HOST_SYSTEM_PROCESSOR}")
    if(_unixodbc_build_triplet STREQUAL "AMD64" OR _unixodbc_build_triplet STREQUAL "x64" OR _unixodbc_build_triplet STREQUAL "x86_64")
        set(_unixodbc_build_triplet "x86_64-pc-linux-gnu")
    elseif(_unixodbc_build_triplet STREQUAL "arm64" OR _unixodbc_build_triplet STREQUAL "ARM64" OR _unixodbc_build_triplet STREQUAL "aarch64")
        set(_unixodbc_build_triplet "aarch64-pc-linux-gnu")
    endif()

    if(_unixodbc_build_triplet)
        list(APPEND UNIXODBC_CONFIGURE_ARGS --build=${_unixodbc_build_triplet})
    endif()
endif()
string(REPLACE ";" " " UNIXODBC_CONFIGURE_ARG_STRING "${UNIXODBC_CONFIGURE_ARGS}")

if(EXISTS "${UNIXODBC_BUILD_DIR}")
    file(REMOVE_RECURSE "${UNIXODBC_BUILD_DIR}")
endif()
file(MAKE_DIRECTORY "${UNIXODBC_BUILD_DIR}")
file(COPY "${UNIXODBC_SOURCE_DIR}/" DESTINATION "${UNIXODBC_BUILD_DIR}" USE_SOURCE_PERMISSIONS)

# Delete conflict flag ACLOCAL_AMFLAGS
file(READ "${UNIXODBC_BUILD_DIR}/Makefile.am" FILE_CONTENTS)
string(REPLACE "ACLOCAL_AMFLAGS=-I m4\n\n" "" FILE_CONTENTS "${FILE_CONTENTS}")
file(WRITE "${UNIXODBC_BUILD_DIR}/Makefile.am" "${FILE_CONTENTS}")

execute_process(
    COMMAND ${CMAKE_COMMAND} -E env
        ${POSIX_SHELL_EXECUTABLE} -c "autoreconf -fi"
    WORKING_DIRECTORY "${UNIXODBC_BUILD_DIR}"
)

# file(COPY) gives every file a fresh, effectively-simultaneous mtime, which
# loses the dependency ordering a real "make dist" tarball would have. Automake's
# maintainer-mode rules then see e.g. configure.ac as not-clearly-older-than
# configure and re-invoke the system's aclocal/autoconf/automake (via the
# "missing" wrapper) to "refresh" them. That regenerates aclocal.m4 against the
# system's (newer) libtool.m4 while the already-built ./libtool script/objects
# still match the vendored (older) one, causing:
#   "libtool: Version mismatch error. This is libtool 2.4.7 ..., but the
#    definition of this LT_INIT comes from libtool 2.4.6."
# Restore a sane dependency order (sources -> aclocal.m4 -> configure/config
# headers -> Makefile.in) for the top-level tree and the libltdl subtree so
# the auto-reconfiguration never triggers.
function(_unixodbc_touch)
    foreach(_unixodbc_touch_file ${ARGN})
        if(EXISTS "${_unixodbc_touch_file}")
            file(TOUCH "${_unixodbc_touch_file}")
        endif()
    endforeach()
endfunction()

function(_unixodbc_fix_autotools_timestamps dir header_in)
    file(GLOB _unixodbc_m4_files "${dir}/m4/*.m4")
    _unixodbc_touch("${dir}/acinclude.m4" "${dir}/Makefile.am" ${_unixodbc_m4_files} "${dir}/configure.ac")
    _unixodbc_touch("${dir}/aclocal.m4")
    _unixodbc_touch("${dir}/configure" "${dir}/${header_in}")
    _unixodbc_touch("${dir}/Makefile.in")
endfunction()

_unixodbc_fix_autotools_timestamps("${UNIXODBC_BUILD_DIR}" "config.h.in")
_unixodbc_fix_autotools_timestamps("${UNIXODBC_BUILD_DIR}/libltdl" "config-h.in")

# The vendored configure script embeds bare carriage returns (0x0D, no
# following 0x0A) as autoconf's internal "protected newline" placeholder for
# multi-line macro arguments (e.g. the AC_CONFIG_FILES list). They should
# have been translated back to real newlines when configure was generated,
# but were left as-is; the script has no genuine CRLF line endings, so it is
# safe to turn every bare CR into a real newline.
# Once normalized, the AC_CONFIG_FILES list still starts with a blank line
# (from "AC_CONFIG_FILES([" being immediately followed by a newline), which
# was never stripped. That leaves a spurious empty config target whose file
# lookup resolves to just ".in", making config.status abort immediately with:
#   "config.status: error: cannot find input file: `.in'"
# Drop that empty target from both the runtime target list and the
# case-statement entry it generates so config.status only sees real files.
set(_unixodbc_configure_file "${UNIXODBC_BUILD_DIR}/configure")
file(READ "${_unixodbc_configure_file}" _unixodbc_configure_contents)
string(REPLACE "\r" "\n" _unixodbc_configure_contents "${_unixodbc_configure_contents}")
string(REPLACE
    "ac_config_files=\"$ac_config_files \n Makefile"
    "ac_config_files=\"$ac_config_files Makefile"
    _unixodbc_configure_contents "${_unixodbc_configure_contents}")
string(REPLACE
    "    \"\n\") CONFIG_FILES=\"$CONFIG_FILES \n\" ;;\n    \"Makefile"
    "    \"Makefile"
    _unixodbc_configure_contents "${_unixodbc_configure_contents}")
# file(WRITE) translates embedded "\n" into CRLF on Windows, which would
# reintroduce the very same "stray CR" problem across the whole script.
# file(GENERATE) writes the requested NEWLINE_STYLE verbatim, so use it to
# keep the script LF-only.
file(GENERATE OUTPUT "${_unixodbc_configure_file}" CONTENT "${_unixodbc_configure_contents}" NEWLINE_STYLE UNIX)

add_custom_target(unixodbc_project

    COMMAND ${CMAKE_COMMAND} -E env
        "CC=${UNIXODBC_C_COMPILER_BASH}"
        "CXX=${UNIXODBC_CXX_COMPILER_BASH}"
        "CPPFLAGS=${_unixodbc_cppflags}"
        "LDFLAGS=${_unixodbc_ldflags}"
        "${POSIX_SHELL_EXECUTABLE}"
        "${UNIXODBC_BUILD_DIR_BASH}/configure"
        ${UNIXODBC_CONFIGURE_ARGS}

    # config.status leaves unixodbc.h/unixodbc_conf.h's "#undef SYMBOL" lines
    # unsubstituted even though config.h gets the real values, which makes
    # sqltypes.h's "#error ... size of a long int" check fail. Copy the real
    # values over after configure has generated config.h.
    COMMAND ${CMAKE_COMMAND} "-DUNIXODBC_DIR=${UNIXODBC_BUILD_DIR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/sync-header-defines.cmake"

    # "make" is a WSL/POSIX binary, not something Windows' cmake -E env can
    # exec directly; route it through the POSIX shell like configure above.
    COMMAND ${CMAKE_COMMAND} -E env "CC=${UNIXODBC_C_COMPILER_BASH}" "CXX=${UNIXODBC_CXX_COMPILER_BASH}" "${POSIX_SHELL_EXECUTABLE}" -c "make -j${CMAKE_BUILD_PARALLEL_LEVEL}"

    COMMAND ${CMAKE_COMMAND} -E env "CC=${UNIXODBC_C_COMPILER_BASH}" "CXX=${UNIXODBC_CXX_COMPILER_BASH}" "${POSIX_SHELL_EXECUTABLE}" -c "make install"

    WORKING_DIRECTORY "${UNIXODBC_BUILD_DIR}"
    BYPRODUCTS "${UNIXODBC_INSTALL_DIR}/lib/libodbc${CMAKE_SHARED_LIBRARY_SUFFIX}"
    VERBATIM
)


set(_unixodbc_include_dir "${UNIXODBC_BUILD_DIR};${UNIXODBC_SOURCE_DIR}/include")
set(_unixodbc_lib_path "${UNIXODBC_INSTALL_DIR}/lib/libodbc.so.2.0.0")
if(NOT EXISTS "${_unixodbc_lib_path}")
    set(_unixodbc_lib_path "${UNIXODBC_INSTALL_DIR}/lib/libodbc${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()
if(NOT EXISTS "${_unixodbc_lib_path}")
    set(_unixodbc_lib_path "${UNIXODBC_INSTALL_DIR}/lib/libodbc.so")
endif()

add_library(unixodbc_library_target SHARED IMPORTED GLOBAL)
set_target_properties(unixodbc_library_target PROPERTIES
    IMPORTED_LOCATION "${_unixodbc_lib_path}"
    INTERFACE_INCLUDE_DIRECTORIES "${_unixodbc_include_dir}"
)
add_dependencies(unixodbc_library_target unixodbc_project)

add_library(ODBC::ODBC ALIAS unixodbc_library_target)
