if(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0")
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
endif()

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(toluapp)

# Force-include the Lua 5.1 compatibility shim so that toluapp sources
# can be compiled against Lua 5.2+ without modification.
set(_COMPAT_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/lua51compat.h")

foreach(_target toluapp_lib toluapp_lib_static toluapp)
    if(TARGET ${_target})
        if(MSVC)
            target_compile_options(${_target} PRIVATE "/FI${_COMPAT_HEADER}")
        else()
            # GCC and Clang require -include and the header path to be
            # passed as two separate arguments.
            target_compile_options(${_target} PRIVATE "-include" "${_COMPAT_HEADER}")
        endif()
    endif()
endforeach()
