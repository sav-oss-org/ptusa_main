# Build identification (issue #1647).
#
# Collects the git commit and tree state at configure time and writes
# them into a generated header plus a BUILDINFO.txt installed next to
# the binaries. Only one small translation unit includes the generated
# header, so a new commit never rebuilds the whole tree.

find_package(Git QUIET)

set(PTUSA_GIT_SHA "unknown")
set(PTUSA_GIT_REF "unknown")
set(PTUSA_GIT_TAG "")
set(PTUSA_GIT_DIRTY 0)
set(PTUSA_BUILD_DATE "")

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

    # Date shown in parentheses next to the version (gh CLI style).
    # The commit date, unlike the configure date, keeps the version
    # text identical for every rebuild of the same sources.
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" log -1 --format=%cd --date=short
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE PTUSA_BUILD_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET RESULT_VARIABLE date_result)
    if(NOT date_result EQUAL 0)
        set(PTUSA_BUILD_DATE "")
    endif()

    # Tag that points exactly at the checked-out commit; empty for
    # any other build. Tagless checkouts keep the commit link.
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" describe --tags --exact-match HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE PTUSA_GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET RESULT_VARIABLE tag_result)
    if(NOT tag_result EQUAL 0)
        set(PTUSA_GIT_TAG "")
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

if(PTUSA_BUILD_DATE STREQUAL "")
    # Source tree without git: fall back to the configure date.
    string(TIMESTAMP PTUSA_BUILD_DATE "%Y-%m-%d" UTC)
endif()

# Repository URL as the checkout sees it (origin), normalized to a
# web address; empty when no remote exists.
set(PTUSA_REPO_URL "")
set(PTUSA_COMMIT_URL "")
if(GIT_FOUND)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" remote get-url origin
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE repo_url
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET RESULT_VARIABLE repo_result)
    if(repo_result EQUAL 0 AND NOT repo_url STREQUAL "")
        string(REGEX REPLACE "\.git$" "" repo_url "${repo_url}")
        string(REGEX REPLACE "^git@github\.com:" "https://github.com/"
            repo_url "${repo_url}")
        set(PTUSA_REPO_URL "${repo_url}")
    endif()
endif()
if(NOT PTUSA_REPO_URL STREQUAL "" AND
        NOT PTUSA_GIT_SHA STREQUAL "unknown")
    set(PTUSA_COMMIT_URL "${PTUSA_REPO_URL}/commit/${PTUSA_GIT_SHA}")
endif()

# URL line under --version: the release page when the build is an
# exact tagged release (as the gh CLI prints), the commit page
# otherwise, empty when no repository address is configured.
set(PTUSA_INFO_URL "")
if(NOT PTUSA_REPO_URL STREQUAL "")
    if(NOT PTUSA_GIT_TAG STREQUAL "")
        set(PTUSA_INFO_URL
            "${PTUSA_REPO_URL}/releases/tag/${PTUSA_GIT_TAG}")
    elseif(NOT PTUSA_GIT_SHA STREQUAL "unknown")
        set(PTUSA_INFO_URL "${PTUSA_COMMIT_URL}")
    endif()
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
