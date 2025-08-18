include_guard(GLOBAL)

mdsplus_option(
    RELEASE_TAG STRING
    "The release tag to use while building in the format of BRANCH_release-MAJOR-MINOR-PATCH. This will default to `git describe --tag` \
    if not specified. This is only meant to be used if you downloaded an archive of the repository, rather than cloning with git."
)

if(NOT DEFINED RELEASE_TAG OR RELEASE_TAG STREQUAL "")

    find_package(Git)

    if(GIT_FOUND)

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
        git(GIT_REMOTE_URL  config remote.${GIT_REMOTE}.url)
        git(GIT_COMMIT      rev-parse HEAD)
        git(GIT_COMMIT_DATE log -1 --format=%ad)

        if(NOT GIT_REMOTE)
            set(GIT_REMOTE "LOCAL")
            set(GIT_REMOTE_URL "file://${CMAKE_SOURCE_DIR}")
        endif()

        # TODO: Remove
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

        set(RELEASE_TAG "${GIT_TAG}")

    else()
        set(RELEASE_TAG "unknown_release-0-0-0")
    endif()

endif()

# We parse it very carefully to avoid breaking branches with - or _ in their name
string(REPLACE "_release-" ";" _branch_version_list "${RELEASE_TAG}") # branch_release-1-2-3 -> branch;1-2-3
list(GET _branch_version_list 0 RELEASE_BRANCH)
list(GET _branch_version_list 1 _version_list)

string(REPLACE "-" ";" _version_list "${_version_list}") # 1-2-3 -> 1;2;3
list(GET _version_list 0 RELEASE_MAJOR)
list(GET _version_list 1 RELEASE_MINOR)
list(GET _version_list 2 RELEASE_RELEASE)

if(DEFINED GIT_BRANCH AND NOT RELEASE_BRANCH STREQUAL GIT_BRANCH)
    message(WARNING "The branch found while parsing the release tag `${RELEASE_BRANCH}` does not match the current git branch `${GIT_BRANCH}`, `${RELEASE_BRANCH}` will be used for RELEASE_BRANCH.")
endif()

set(RELEASE_VERSION "${RELEASE_MAJOR}.${RELEASE_MINOR}.${RELEASE_RELEASE}")

# Set the standard CMake version variables
set(PROJECT_VERSION_MAJOR ${RELEASE_MAJOR})
set(PROJECT_VERSION_MINOR ${RELEASE_MINOR})
set(PROJECT_VERSION_PATCH ${RELEASE_RELEASE})
set(PROJECT_VERSION ${RELEASE_VERSION})

# TODO: Remove
add_custom_target(
    release_tag
    COMMENT ""
    COMMAND ${CMAKE_COMMAND} -E echo "${RELEASE_TAG}"
)
