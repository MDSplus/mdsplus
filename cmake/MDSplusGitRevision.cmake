include_guard(GLOBAL)

find_package(Git)

find_program(date_EXECUTABLE date)
mark_as_advanced(date_EXECUTABLE)

# Convenience macro for all the git queries we need to make
macro(git _output_variable) # additional arguments are in ${ARGN}
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" ${ARGN}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE ${_output_variable}
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT _result EQUAL 0)
        SET(${_output_variable} "NOTFOUND") # NOTFOUND is CMake's version of NULL
    endif()
endmacro()

git(GIT_TAG         describe --tag)
git(GIT_BRANCH      rev-parse --abbrev-ref HEAD)
git(GIT_REMOTE      config branch.${GIT_BRANCH}.remote)
git(GIT_REMOTE_URL  config branch.${GIT_BRANCH}.url)
git(GIT_COMMIT      rev-parse HEAD)
git(GIT_COMMIT_DATE log -1 --format=%ad)

if(NOT GIT_REMOTE)
    set(GIT_REMOTE "LOCAL")
    set(GIT_REMOTE_URL "file:/${CMAKE_SOURCE_DIR}")
endif()

if(RELEASE_VERSION)
    string(REPLACE "." ";" _version_list "${RELEASE_VERSION}") # 1.2.3 -> 1;2;3
    list(GET _version_list 0 RELEASE_MAJOR)
    list(GET _version_list 1 RELEASE_MINOR)
    list(GET _version_list 2 RELEASE_RELEASE)
endif()

set(RELEASE_TAG "${GIT_TAG}")

if(NOT RELEASE_MAJOR OR NOT RELEASE_MINOR OR NOT RELEASE_RELEASE)
    string(REPLACE "-" ";" _version_list "${RELEASE_TAG}") # branch-1-2-3 -> branch;1;2;3
    list(GET _version_list 1 RELEASE_MAJOR)
    list(GET _version_list 2 RELEASE_MINOR)
    list(GET _version_list 3 RELEASE_RELEASE)
endif()

if(NOT RELEASE_MAJOR OR NOT RELEASE_MINOR OR NOT RELEASE_RELEASE)
    set(RELEASE_MAJOR 0)
    set(RELEASE_MINOR 0)
    set(RELEASE_RELEASE 0)
endif()

set(RELEASE_VERSION "${RELEASE_MAJOR}.${RELEASE_MINOR}.${RELEASE_RELEASE}")

if(date_EXECUTABLE)
    execute_process(
        COMMAND "${date_EXECUTABLE}"
        OUTPUT_VARIABLE RELEASE_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    # STRING(TIMESTMAP) is missing AM/PM and the Timezone, but it is a sensible fallback
    string(TIMESTAMP RELEASE_DATE "%a %d %b %Y %H:%M:%S UTC" UTC)
endif()

if(DEFINED ENV{BRANCH})
    string(SUBSTRING "$ENV{BRANCH}" 0 12 RELEASE_BRANCH)
endif()

if(NOT RELEASE_BRANCH)
    string(SUBSTRING "${GIT_BRANCH}" 0 12 RELEASE_BRANCH)
endif()

if(NOT RELEASE_BRANCH)
    set(RELEASE_BRANCH "unknown")
endif()

set(RELEASE_TAG "${RELEASE_BRANCH}_release-${RELEASE_MAJOR}-${RELEASE_MINOR}-${RELEASE_RELEASE}")

add_custom_target(
    release_tag
    COMMENT ""
    COMMAND ${CMAKE_COMMAND} -E echo "${RELEASE_TAG}"
)

# TODO:
# 	@ \\\$(if \\\$(_git_check_changes),\\\$(info  WARNING: There are uncommitted changes))\\

add_custom_target(
    git_info
    COMMENT ""
    COMMAND ${CMAKE_COMMAND} -E echo " Git repository status: "
    COMMAND ${CMAKE_COMMAND} -E echo " ------------------------------ "
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "   tag: ...... ${GIT_TAG}"
    COMMAND ${CMAKE_COMMAND} -E echo "   branch: ... ${GIT_BRANCH}"
    COMMAND ${CMAKE_COMMAND} -E echo "   commit: ... ${GIT_COMMIT}"
    COMMAND ${CMAKE_COMMAND} -E echo "   remote_url: ${GIT_REMOTE_URL}"
    COMMAND ${CMAKE_COMMAND} -E echo ""
)

# There's no way to completely remove the extra CMake output from the git_info and release_tag targets
# However, we can write this same info into files instead

file(WRITE ${CMAKE_BINARY_DIR}/release_tag
    "${RELEASE_TAG}\n"
)

file(WRITE ${CMAKE_BINARY_DIR}/git_info
    " Git repository status: \n"
    " ------------------------------ \n"
    "\n"
    "   tag: ...... ${GIT_TAG}\n"
    "   branch: ... ${GIT_BRANCH}\n"
    "   commit: ... ${GIT_COMMIT}\n"
    "   remote_url: ${GIT_REMOTE_URL}\n"
    "\n"
)