cmake_minimum_required(VERSION 3.31)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)

project(ptusa_main VERSION 2026.08.21.0)

message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "CMake executable: ${CMAKE_COMMAND}")
message(STATUS "CMake configured from: ${CMAKE_BINARY_DIR}")

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
################# create targets ##############################################

if(LINUX)
    add_compile_definitions(LINUX_OS)
elseif(WIN32)
    add_compile_definitions(WIN_OS)
    add_compile_definitions(SIMPLE_LOG)
endif()

add_compile_definitions(TEST_SPEED MODBUS)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/$<CONFIG>")

if(MSVC)
    add_compile_options("/utf-8")

    # Исключить конфликтующую библиотеку
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMTD")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMTD")
endif(MSVC)

configure_file( version_info.in version_info.h @ONLY)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(deps/fmt EXCLUDE_FROM_ALL)
set(BUILD_SHARED_LIBS ON)

add_subdirectory(deps/lua EXCLUDE_FROM_ALL)

# Use the standard Windows Lua 5.1 DLL name (lua51.dll) so that ptusa_main.dll
# is compatible with external Lua 5.1 runtimes (e.g., EPLANner) that use
# lua51.dll instead of lua.dll.
if(WIN32)
    set_target_properties(liblua PROPERTIES OUTPUT_NAME lua51)
endif()

set(LUA_LIBRARIES liblua)
set(LUA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/lua/lua/src;${CMAKE_BINARY_DIR}/deps/lua/lua)

add_subdirectory(deps/toluapp EXCLUDE_FROM_ALL)

add_subdirectory(deps/zlib EXCLUDE_FROM_ALL)

# Google Test configuration.
set(INSTALL_GTEST OFF)
set(INSTALL_GMOCK OFF)
# Build as static libraries to avoid DLL export issues.
set(BUILD_SHARED_LIBS OFF)
# Use dynamic CRT (/MD) to match the main project.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Скрыть внутренние символы для уменьшения размера бинарника.
set(gtest_hide_internal_symbols ON CACHE BOOL "" FORCE)
add_subdirectory(deps/googletest EXCLUDE_FROM_ALL)
set(BUILD_SHARED_LIBS ON)

add_subdirectory(deps/subhook EXCLUDE_FROM_ALL)

# Google Benchmark configuration.
if(NOT MINGW)
    # Не собирать внутренние тесты benchmark.
    set(BENCHMARK_ENABLE_TESTING OFF)
    # Не использовать libc++ (MSVC использует свою STL).
    set(BENCHMARK_USE_LIBCXX OFF)
    # Собирать как статическую библиотеку.
    set(BUILD_SHARED_LIBS OFF)
    # Не создавать install targets.
    set(BENCHMARK_ENABLE_INSTALL OFF)
    if(WIN32)
        # Отключить GTest-тесты benchmark (требуют pthreads).
        set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
        # Использовать Win32 threads вместо pthreads.
        set(CMAKE_USE_WIN32_THREADS_INIT ON)
    endif()
    add_subdirectory(deps/benchmark EXCLUDE_FROM_ALL)
    set(BUILD_SHARED_LIBS ON)
endif()

add_subdirectory(deps/open62541 EXCLUDE_FROM_ALL)

add_subdirectory(deps/cxxopts EXCLUDE_FROM_ALL)

add_subdirectory(deps/lunarmodules/luafilesystem EXCLUDE_FROM_ALL)
add_subdirectory(deps/lunarmodules/luasystem EXCLUDE_FROM_ALL)
add_subdirectory(deps/lunarmodules/term EXCLUDE_FROM_ALL)
add_subdirectory(deps/lunarmodules/luassert EXCLUDE_FROM_ALL)

add_subdirectory(deps/lunarmodules/luasql EXCLUDE_FROM_ALL)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ToluaCopyIfDifferent.cmake")

file(GLOB common_src CONFIGURE_DEPENDS
        common/PAC-driver/*.cpp
        common/util/*.c
        common/util/*.cpp
        common/*.cpp

        PAC/common/*.cpp
        PAC/common/sys/*.cpp
        PAC/common/device/*.cpp
        PAC/IoT/*.cpp)

file(GLOB test_src CONFIGURE_DEPENDS
            test/*.cpp
            test/device/*.cpp
			test/sys/*.cpp
            test/dll/*.cpp)

if(LINUX)
    file(GLOB linux_src CONFIGURE_DEPENDS
            PAC/linux/*.cpp
            PAC/linux/PAC_PC/*.cpp)
endif()

if(WIN32)
    file(GLOB win_src CONFIGURE_DEPENDS
            common/win_util/*.cpp

            PAC/win/*.cpp
            PAC/win/PAC_PC/*.cpp)
endif()

if(LINUX)
    add_executable(${PROJECT_NAME} ${common_src} ${linux_src}
            PAC_control_projects/main_control_prg/main_cycle.cpp
            PAC_control_projects/main_control_prg/main.cpp )

    add_library(libptusa_main SHARED ${common_src} ${linux_src}
            PAC_control_projects/main_control_prg/main_control_dll_vc/main_control_dll_vc.cpp)

    add_executable(main_test ${common_src} ${linux_src} ${test_src}
           PAC_control_projects/main_control_prg/main_cycle.cpp
           PAC_control_projects/main_control_prg/main_control_dll_vc/main_control_dll_vc.cpp
           test/main_test.cpp)

    add_executable(main_performance_test ${common_src} ${linux_src}
           test/performance/main_performance_test.cpp
           PAC_control_projects/main_control_prg/version.rc)

    if(ARP_DEVICE)
        execute_process(COMMAND plcncli
            generate code -s . -o ${CMAKE_BINARY_DIR}/PtusaPLCnextEngineer/intermediate/code --verbose
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext/PtusaPLCnextEngineer
            COMMAND_ECHO STDOUT )
        execute_process(COMMAND plcncli
            generate config -s . -o ${CMAKE_BINARY_DIR}/PtusaPLCnextEngineer/intermediate/config --verbose
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext/PtusaPLCnextEngineer
            COMMAND_ECHO STDOUT )

        file(GLOB PLCnext_engineer_src CONFIGURE_DEPENDS
            PAC/linux/PAC_PLCnext/PtusaPLCnextEngineer/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext/PtusaPLCnextEngineer/*.cpp
            ${CMAKE_BINARY_DIR}/PtusaPLCnextEngineer/intermediate/code/*.cpp )

        add_library(PtusaPLCnextEngineer SHARED
            ${common_src} ${linux_src} ${PLCnext_engineer_src}
            PAC_control_projects/main_control_prg/main_cycle.cpp )
    endif()

elseif(WIN32)
    add_executable(${PROJECT_NAME} ${common_src} ${win_src}
            PAC_control_projects/main_control_prg/main_cycle.cpp
            PAC_control_projects/main_control_prg/main.cpp
            PAC_control_projects/main_control_prg/version.rc)

    add_library(libptusa_main SHARED ${common_src} ${win_src}
            PAC_control_projects/main_control_prg/main_control_dll_vc/main_control_dll_vc.cpp
            PAC_control_projects/main_control_prg/main_control_dll_vc/dllmain.cpp
            PAC_control_projects/main_control_prg/version.rc)

    add_executable(main_test ${common_src} ${win_src} ${test_src}
           test/main_test.cpp
           PAC_control_projects/main_control_prg/main_cycle.cpp
           PAC_control_projects/main_control_prg/main_control_dll_vc/main_control_dll_vc.cpp
           PAC_control_projects/main_control_prg/version.rc)

    if(NOT MINGW)
        add_executable(main_performance_test ${common_src} ${win_src}
               test/performance/main_performance_test.cpp
               PAC_control_projects/main_control_prg/version.rc)
    endif()

    regenerate_tolua_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/PAC_dev_lua.cpp"
        PAC_dev
        "${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/toloa++/pkg/PAC_dev.hh")
endif()

if(NOT ARP_DEVICE)
    add_dependencies(libptusa_main lua)
endif()

add_dependencies(lua lfs luasystem term_core luassert_copy luasql_odbc)

if(MSVC)
    set_property(TARGET libptusa_main PROPERTY VS_DEBUGGER_COMMAND "lua.exe")
endif()

target_compile_definitions(main_test PUBLIC DEBUG_NO_IO_MODULES)
target_compile_definitions(libptusa_main PUBLIC DEBUG_NO_IO_MODULES)
if(NOT MINGW)
    target_compile_definitions(main_performance_test PUBLIC DEBUG_NO_IO_MODULES)
endif()

if(ARP_DEVICE)
    target_compile_definitions(PtusaPLCnextEngineer PUBLIC PAC_PLCNEXT)
    target_compile_definitions(ptusa_main PUBLIC PAC_PC)
    target_compile_definitions(libptusa_main PUBLIC PAC_PC)
    target_compile_definitions(main_performance_test PUBLIC PAC_PC)
else()
    add_compile_definitions(PAC_PC)
endif()

if(ARP_DEVICE OR MINGW)
    list(INSERT CMAKE_MODULE_PATH 0 "${ARP_TOOLCHAIN_CMAKE_MODULE_PATH}")

    set_target_properties(main_test PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()

option(CODE_COVERAGE "Enable coverage reporting" OFF)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # Add required flags (GCC & LLVM/Clang)
  target_compile_options(main_test PUBLIC
    -O0        # no optimization
    -g         # generate debug info
    --coverage # sets all required flags
    )

  target_link_libraries(main_test gcov)
  target_link_options(main_test PUBLIC --coverage)
endif()

set_target_properties (libptusa_main PROPERTIES OUTPUT_NAME ptusa_main PREFIX "")
target_compile_definitions(libptusa_main PUBLIC _USRDLL)

target_compile_definitions(main_test PUBLIC PTUSA_TEST)

###############################################################################

################# project include-paths #######################################

set(GENERAL_INCLUDES
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/util>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/PAC-driver>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/sys>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC_control_projects/main_control_prg>

        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>

if (WIN32)
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/win_util>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/win>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/win/PAC_PC>
elseif (LINUX)
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PC>

if(ARP_DEVICE)
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext/PtusaPLCnextEngineer>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/PtusaPLCnextEngineer/intermediate/code>
endif()

endif ()
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/open62541/open62541/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/open62541/open62541/plugins/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/open62541/open62541/arch>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/open62541/open62541/deps>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/deps/open62541/open62541/src_generated>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/IoT>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/toluapp/toluapp/include>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/lua/lua/src>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/deps/lua/lua>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib/zlib>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/deps/zlib/zlib>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/fmt/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/cxxopts/cxxopts/include>)

target_include_directories(${PROJECT_NAME} PUBLIC ${GENERAL_INCLUDES})
target_include_directories(libptusa_main PUBLIC ${GENERAL_INCLUDES})
if(ARP_DEVICE)
    target_include_directories(PtusaPLCnextEngineer PUBLIC ${GENERAL_INCLUDES})
endif()

set(TEST_INCLUDES
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/deps/googletest/googletest/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/deps/googletest/googletest>

        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/deps/googletest/googlemock/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/deps/googlemock/googlemock>)

target_include_directories(main_test PUBLIC ${GENERAL_INCLUDES} ${TEST_INCLUDES})
if(NOT MINGW)
    target_include_directories(main_performance_test PUBLIC ${GENERAL_INCLUDES})
endif()

###############################################################################

################# set link options ############################################
# WARNING:                                                                    #
# Without --no-undefined the linker will not check, whether all necessary     #
# libraries are linked. When a library which is necessary is not linked,      #
# the firmware will crash and there will be NO indication why it crashed.     #
###############################################################################
if(LINUX)
    target_link_options(${PROJECT_NAME} PRIVATE LINKER:--no-undefined)
	target_link_options(${PROJECT_NAME} PRIVATE LINKER:--as-needed)
endif()
###############################################################################

if(ARP_DEVICE)
    set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH $ORIGIN)
endif()

################# add link targets ############################################
if(ARP_DEVICE)
    find_package(ArpDevice REQUIRED)

    message(STATUS "ARP_DEVICE_VERSION=${ARP_DEVICE_VERSION}")
	message(STATUS "ARP_DEVICE_SHORT_VERSION=${ARP_DEVICE_SHORT_VERSION}")
	message(STATUS "ARP_DEVICE_SHORT_VERSION_BUILD=${ARP_DEVICE_SHORT_VERSION_BUILD}")

	if(${ARP_VERSION_MAJOR} GREATER_EQUAL "25")
		find_package(Arp REQUIRED)
		target_link_libraries(PtusaPLCnextEngineer PRIVATE Arp::ALL)
	else()
		find_package(ArpProgramming REQUIRED)
		target_link_libraries(PtusaPLCnextEngineer PRIVATE ArpDevice ArpProgramming)
	endif()

    target_link_libraries(PtusaPLCnextEngineer PRIVATE
		liblua_static toluapp_lib_static zlibstatic
		fmt::fmt open62541::open62541)
endif()

################# add link targets ############################################

target_link_libraries(${PROJECT_NAME} PRIVATE liblua_static toluapp_lib_static zlibstatic fmt::fmt)
target_link_libraries(libptusa_main toluapp_lib_static zlibstatic fmt::fmt)

target_link_libraries(${PROJECT_NAME} PRIVATE open62541::open62541)
target_link_libraries(libptusa_main open62541::open62541)
target_link_libraries(main_test open62541::open62541)
if(NOT MINGW)
    target_link_libraries(main_performance_test open62541::open62541)
endif()

target_link_libraries(main_test liblua_static toluapp_lib_static zlibstatic GTest::gtest GTest::gmock subhook fmt::fmt)

if(NOT MINGW)
    target_link_libraries(main_performance_test liblua_static toluapp_lib_static zlibstatic fmt::fmt benchmark::benchmark)
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE wsock32 ws2_32)
    target_link_libraries(libptusa_main liblua wsock32 ws2_32)
    target_link_libraries(main_test wsock32 ws2_32)
    if(NOT MINGW)
        target_link_libraries(main_performance_test wsock32 ws2_32)
    endif()

elseif (LINUX)
    target_link_libraries(libptusa_main liblua_static)
endif()

###############################################################################
if(ARP_DEVICE)
    string(REGEX REPLACE "^.*\\(([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+).*$" "\\1" _ARP_SHORT_DEVICE_VERSION ${ARP_DEVICE_VERSION})
    install(TARGETS ${PROJECT_NAME} libptusa_main PtusaPLCnextEngineer
        LIBRARY DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/lib
        ARCHIVE DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/lib
        RUNTIME DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/bin)

    install(CODE "execute_process(COMMAND plcncli
                deploy -s \"${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext/PtusaPLCnextEngineer\"
                -t \"${ARP_DEVICE},${ARP_DEVICE_VERSION}\"
                -f \"${CMAKE_BINARY_DIR}/\${CMAKE_INSTALL_CONFIG_NAME}/libPtusaPLCnextEngineer.so|.\"
                --verbose
            WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}/PtusaPLCnextEngineer\"
            COMMAND_ECHO STDOUT )")
     install(FILES ${CMAKE_BINARY_DIR}/PtusaPLCnextEngineer/bin/PtusaPLCnextEngineer.pcwlx
        DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>)

    unset(_ARP_SHORT_DEVICE_VERSION)
else()
    install(TARGETS ${PROJECT_NAME} libptusa_main
        LIBRARY DESTINATION $<CONFIG>
        RUNTIME DESTINATION $<CONFIG>)
endif()

enable_testing()
add_test(run_no_command_line_arguments ${PROJECT_NAME})
if (WIN32)
    set_tests_properties( run_no_command_line_arguments
        PROPERTIES PASS_REGULAR_EXPRESSION
[[
(.*)
  -v, --version          Print version info
  -d, --debug            Enable debugging
      --no_io            No communicate with I\\O nodes \(default: true\)
      --read_only_io     Read only from I\\O nodes \(default: true\)
  -p, --port arg         Param port \(default: 10000\)
  -h, --help             Print help info
  -r, --rcrc             Reset params
      --opc arg          OPC UA server behavior \(off, r, rw\)
      --sys_path arg     Sys path \(default: \./sys\)
      --path arg         Path \(default: \.\)
      --extra_paths arg  Extra paths \(default: \./dairy-sys\)
      --sleep_time arg   Sleep time, ms \(default: 2\)
]] )
else()
    set_tests_properties( run_no_command_line_arguments
        PROPERTIES PASS_REGULAR_EXPRESSION
[[
(.*)
  -v, --version          Print version info
  -d, --debug            Enable debugging
      --no_io            No communicate with I\\O nodes
      --read_only_io     Read only from I\\O nodes
  -p, --port arg         Param port \(default: 10000\)
  -h, --help             Print help info
  -r, --rcrc             Reset params
      --opc arg          OPC UA server behavior \(off, r, rw\)
      --sys_path arg     Sys path \(default: \./sys\)
      --path arg         Path \(default: \.\)
      --extra_paths arg  Extra paths \(default: \./dairy-sys\)
      --sleep_time arg   Sleep time, ms \(default: 2\)
]] )
endif()

# Общие пути в Lua package.path
set(LUA_MODULES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/deps/lunarmodules")
set(LUA_BUSTED_PATH_INIT_CMD
    "package.path = package.path..';${LUA_MODULES_PATH}/busted/?/init.lua'")
set(LUA_BUSTED_PATH_FILES_CMD
    "package.path = package.path..';${LUA_MODULES_PATH}/busted/?.lua'")
set(LUA_LIBRARIES_PATH "${PROJECT_BINARY_DIR}/$<CONFIG>/?${CMAKE_SHARED_LIBRARY_SUFFIX}")
set(DEMO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo")
set(LUA_T1_DEMO_EARGS_CMD
    "package.path = package.path..'"
    "./?.lua"
    "${LUA_MODULES_PATH}/penlight/lua/?.lua"
    "${LUA_MODULES_PATH}/luasystem/luasystem/?/init.lua"
    "${LUA_MODULES_PATH}/mediator/src/?.lua"
    "${LUA_MODULES_PATH}/term/term/?/init.lua"
    "${LUA_MODULES_PATH}/term/term/?.lua"
    "${PROJECT_BINARY_DIR}/$<CONFIG>/?.lua" #luassert
    "${PROJECT_BINARY_DIR}/$<CONFIG>/?/init.lua"
    "${LUA_MODULES_PATH}/say/say/src/?/init.lua"
    "${LUA_MODULES_PATH}/dkjson/?.lua"
    "${LUA_MODULES_PATH}/cliargs/src/?.lua'")

if(NOT ARP_DEVICE AND NOT MINGW AND NOT SKIP_DEMO)
    add_test(NAME T1-PLCnext-Demo_main COMMAND ${PROJECT_NAME} main.plua1
		    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo )
    set_tests_properties(T1-PLCnext-Demo_main PROPERTIES
        PASS_REGULAR_EXPRESSION "Error: Script file 'main\.plua1' does not exist.")

    add_test(NAME Busted-test COMMAND lua
                -e "${LUA_BUSTED_PATH_INIT_CMD}"
                -e "${LUA_BUSTED_PATH_FILES_CMD}"
                -e "package.path = package.path..';${LUA_MODULES_PATH}/penlight/lua/?.lua'"
                ${LUA_MODULES_PATH}/busted/busted.lua)

    add_test(NAME T1-PLCnext-Demo COMMAND lua
                -e "package.path = package.path..';./?.lua'"
                -e "${LUA_BUSTED_PATH_INIT_CMD}"
                -e "${LUA_BUSTED_PATH_FILES_CMD}"
                -e "${LUA_T1_DEMO_EARGS_CMD}"
                -e "package.cpath = package.cpath..';${LUA_LIBRARIES_PATH}'"
                ${DEMO_PATH}/spec/ptusa_main.lua
            WORKING_DIRECTORY ${DEMO_PATH})

    if(LINUX)
        add_test(NAME T1-PLCnext-Demo_valgrind COMMAND valgrind
                --error-exitcode=1
                --num-callers=100
                --leak-check=full
                --verbose
                --track-origins=yes
                --show-leak-kinds=all
                "$<TARGET_FILE:lua>"
                -e "${LUA_BUSTED_PATH_INIT_CMD}"
                -e "${LUA_BUSTED_PATH_FILES_CMD}"
                -e "${LUA_T1_DEMO_EARGS_CMD}"
                -e "package.cpath = package.cpath..';${LUA_LIBRARIES_PATH}'"
                ${DEMO_PATH}/spec/ptusa_main.lua
            WORKING_DIRECTORY ${DEMO_PATH})
    endif()


    add_test(NAME save-speed-benchmark COMMAND main_performance_test
            --benchmark_out=${CMAKE_BINARY_DIR}/benchmark_result.json --benchmark_out_format=json
            --benchmark_context=pwd=`${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo`
            --benchmark_counters_tabular=true
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo)

    set_property(TEST save-speed-benchmark PROPERTY RUN_SERIAL 1)
endif()

include(GoogleTest)

set(STDOUT_CAPTURE_LOCKED_FILTER
	"io_device.get_AO:"
	"io_device.get_AO_read_data:"

	"dev_stub.print:"

	"analog_io_device.set_cmd:"

	"watchdog.set_property:"

	"device_communicator.print:"

	"action.print:"

	"operation_state.print:"
	"operation_state.to_step:"
	"operation.on_extra_step_debug_output:"
	"operation.evaluate_from_run_to_pause:"

	"wash_action.print:"

	"PAC_critical_errors_manager.set_global_error:"

	"uni_io_manager.write_outputs:"

	"tech_object.set_mode:"

    "project_manager.proc_main_params:"
    "project_manager.lua_load_configuration:"
	"project_manager.apply_opc_mode:")

set(TCP_SOCKET_LOCKED_FILTER
    "tcp_communicator.evaluate:"
	"tcp_communicator.recvtimeout:"
    "device_communicator.write_devices_states_service:"
    "pp_mode_alarm.pp_mode_activation_deactivation:"
	"pp_mode_alarm.cfg_bus_error_activation_deactivation:"
    "pp_mode_alarm.e_communicate:"
    "main_cycle.main_cycle:"
	"PAC_info.set_cmd:"
	"LuaManagerTest.init_success:"
    "PAC_critical_errors_manager.set_global_error:")

# Тесты, которые используют CaptureStdout() - выполняются последовательно
string(REPLACE ";" "" STDOUT_CAPTURE_LOCKED_FILTER_STR
    "${STDOUT_CAPTURE_LOCKED_FILTER}")

# Join list into a single GTest filter string (elements already end with ':').
string(REPLACE ";" "" TCP_SOCKET_LOCKED_FILTER_STR
	"${TCP_SOCKET_LOCKED_FILTER}")

set(UNLOCKED_FILTER_STR
	"-${TCP_SOCKET_LOCKED_FILTER_STR}${STDOUT_CAPTURE_LOCKED_FILTER_STR}cipline_tech_object.save_device")

# Discover tests that use shared tcp_communicator socket and serialize them.
gtest_discover_tests(main_test
    DISCOVERY_TIMEOUT 30
    TEST_FILTER ${TCP_SOCKET_LOCKED_FILTER_STR}
    PROPERTIES RESOURCE_LOCK tcp_socket)

gtest_discover_tests(main_test
    DISCOVERY_TIMEOUT 30
    TEST_FILTER "cipline_tech_object.save_device"
    PROPERTIES RUN_SERIAL 1)

# Тесты, которые захватывают stdout - выполняются последовательно
# (без параллелизма, чтобы избежать гонки на stdout)
gtest_discover_tests(main_test
    DISCOVERY_TIMEOUT 30
    TEST_FILTER ${STDOUT_CAPTURE_LOCKED_FILTER_STR}
    PROPERTIES RUN_SERIAL 1)

# Discover all remaining tests.
gtest_discover_tests(main_test
    DISCOVERY_TIMEOUT 30
    TEST_FILTER ${UNLOCKED_FILTER_STR})
