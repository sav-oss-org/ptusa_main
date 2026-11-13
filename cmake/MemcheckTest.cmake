# Registers a CTest case that runs a test binary under Valgrind memcheck.
#
# Opt in with the MEMORY_CHECK option. The case is registered only on
# Linux and only when valgrind is found; on every other platform, or
# without valgrind, the function does nothing, so ctest does not even
# see the extra case.

option(MEMORY_CHECK "Run test binaries under Valgrind memcheck" OFF)

function(run_memcheck_test target)
    if(NOT MEMORY_CHECK)
        return()
    endif()

    if(NOT LINUX)
        message(STATUS
            "MEMORY_CHECK=ON is supported on Linux only: skipping it.")
        return()
    endif()

    find_program(VALGRIND_EXECUTABLE valgrind)
    if(NOT VALGRIND_EXECUTABLE)
        message(STATUS
            "valgrind was not found: skipping the ${target}_memcheck test.")
        return()
    endif()

    # Print every leak kind, but fail the case only on definite leaks.
    add_test(NAME ${target}_memcheck
        COMMAND ${VALGRIND_EXECUTABLE}
            --tool=memcheck
            --leak-check=full
            --show-leak-kinds=all
            --errors-for-leak-kinds=definite
            --error-exitcode=1
            --num-callers=40
            --track-origins=yes
            "$<TARGET_FILE:${target}>")

    # The whole binary suite runs at once, so allow a slow memcheck run.
    set_tests_properties(${target}_memcheck PROPERTIES
        TIMEOUT 3600
        RUN_SERIAL TRUE)
endfunction()
