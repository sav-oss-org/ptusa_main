# Build identification (issue #1647).
#
# Collects the git commit and tree state at configure time and writes
# them into a generated header plus a BUILDINFO.txt installed next to
# the binaries. Only one small translation unit includes the generated
# header, so a new commit never rebuilds the whole tree.

find_package(Git QUIET)

set(PTUSA_GIT_SHA "unknown")
set(PTUSA_GIT_REF "unknown")
set(PTUSA_GIT_DIRTY 0)

set(PTUSA_GIT_DIR "${CMAKE_SOURCE_DIR}/.git")
if(GIT_FOUND AND EXISTS "${PTUSA_GIT_DIR}")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE PTUSA_GIT_SHA
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET RESULT_VARIABLE sha_result)
    if(NOT sha_result EQUAL 0)
        set(PTUSA_GIT_SHA "unknown")
    endif()

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" status --porcelain
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE git_status
        ERROR_QUIET RESULT_VARIABLE status_result)
    if(status_result EQUAL 0 AND NOT git_status STREQUAL "")
        set(PTUSA_GIT_DIRTY 1)
    endif()

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE PTUSA_GIT_REF
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET RESULT_VARIABLE ref_result)
    if(NOT ref_result EQUAL 0 OR PTUSA_GIT_REF STREQUAL "HEAD")
        # Detached checkouts (CI merge refs) carry no branch; fall
        # back to the CI ref when present.
        if(DEFINED ENV{GITHUB_REF_NAME})
            set(PTUSA_GIT_REF "$ENV{GITHUB_REF_NAME}")
        else()
            set(PTUSA_GIT_REF "unknown")
        endif()
    endif()

    # Reconfigure when the checked-out commit changes in a plain
    # clone. Git worktrees keep HEAD outside .git and refresh on the
    # next manual reconfigure, which matches the workflow usage.
    if(IS_DIRECTORY "${PTUSA_GIT_DIR}")
        set_property(DIRECTORY APPEND PROPERTY
            CMAKE_CONFIGURE_DEPENDS "${PTUSA_GIT_DIR}/HEAD")
    endif()
endif()

if(PTUSA_GIT_SHA STREQUAL "unknown")
    set(PTUSA_GIT_DIRTY 0)
endif()

set(PTUSA_BUILD_ID "${PROJECT_VERSION}+${PTUSA_GIT_SHA}")
if(PTUSA_GIT_DIRTY)
    string(APPEND PTUSA_BUILD_ID "-dirty")
endif()

configure_file(build_info_data.h.in build_info_data.h @ONLY)

# Continuous integration context lands in BUILDINFO.txt only; it is
# never compiled into the binary, so the build stays reproducible.
foreach(ci_var IN ITEMS GITHUB_SHA GITHUB_REF GITHUB_HEAD_REF
        GITHUB_RUN_ID GITHUB_ACTOR)
    if(DEFINED ENV{${ci_var}})
        set(CI_${ci_var} "$ENV{${ci_var}}")
    else()
        set(CI_${ci_var} "")
    endif()
endforeach()

string(TIMESTAMP PTUSA_CONFIGURE_TIMESTAMP "%Y-%m-%d %H:%M:%S UTC" UTC)
configure_file(BUILDINFO.txt.in BUILDINFO.txt @ONLY)

install(FILES "${CMAKE_BINARY_DIR}/BUILDINFO.txt" DESTINATION ".")

message(STATUS "Build ID: ${PTUSA_BUILD_ID}")
