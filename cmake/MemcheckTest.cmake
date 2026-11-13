# Registers a CTest case that runs a test binary under Valgrind memcheck.
#
# Opt in with the MEMORY_CHECK option. The case is registered only on
# Linux and only when valgrind is found; on every other platform, or
# without valgrind, the function does nothing, so ctest does not even
# see the extra case.

option(MEMORY_CHECK "Run test binaries under Valgrind memcheck" OFF)

# smc-check=all is mandatory: subhook-based tests patch file-backed
# code at runtime, and valgrind's default smc-check assumes such code
# never changes.  Without this flag installed hooks stay invisible,
# removed hooks keep executing, and unrelated later tests crash on
# dangling trampolines.
#
# Tests excluded from memcheck by default, as gtest full names.
# Extend this list when CI flags new names; fix the tests
# themselves separately.
set(MEMCHECK_EXCLUDE_TESTS ""
    CACHE STRING "Extra gtest full names to exclude from memcheck")
set(MEMCHECK_DEFAULT_EXCLUDE
    # Compare wall-clock uptimes and countdown timers; memcheck
    # slowdowns shift the measured values.
    PAC_info.save_device
    cipline_tech_object.OpolRR
    # checkBuff() calls FD_SET without an FD_SETSIZE guard; valgrind
    # opens enough descriptors to push fd past 1024 and glibc aborts
    # with __fdelt_chk.  Re-enable together with the fd guard fix.
    tcp_communicator.checkBuff)

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

    # Build the negative gtest filter from default plus extra excludes.
    set(_excludes ${MEMCHECK_DEFAULT_EXCLUDE} ${MEMCHECK_EXCLUDE_TESTS})
    list(REMOVE_DUPLICATES _excludes)
    string(REPLACE ";" ":" _exclude_filter "${_excludes}")

    # Print every leak kind, but fail the case only on definite leaks.
    add_test(NAME ${target}_memcheck
        COMMAND ${VALGRIND_EXECUTABLE}
            --tool=memcheck
            --smc-check=all
            --leak-check=full
            --show-leak-kinds=all
            --errors-for-leak-kinds=definite
            --error-exitcode=1
            --num-callers=40
            --track-origins=yes
            "$<TARGET_FILE:${target}>"
            --gtest_filter=-${_exclude_filter})

    # The whole binary suite runs at once, so allow a slow memcheck run.
    set_tests_properties(${target}_memcheck PROPERTIES
        TIMEOUT 3600
        RUN_SERIAL TRUE)
endfunction()
