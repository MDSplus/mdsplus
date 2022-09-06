
INCLUDE(GetDate)

IF(NOT GIT_FOUND)
    FIND_PACKAGE(Git QUIET)
ENDIF()

MACRO(GIT _output_variable)
    EXECUTE_PROCESS(
        COMMAND "${GIT_EXECUTABLE}" ${ARGN}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE ${_output_variable}
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    IF (NOT _result EQUAL 0)
        SET(${_output_variable} "NOTFOUND")
    ENDIF()
ENDMACRO()

GIT(GIT_TAG         describe --tag)
GIT(GIT_BRANCH      rev-parse --abbrev-ref HEAD)
GIT(GIT_REMOTE      config branch.${GIT_BRANCH}.remote)
GIT(GIT_REMOTE_URL  config branch.${GIT_BRANCH}.url)
GIT(GIT_COMMIT      rev-parse HEAD)
GIT(GIT_COMMIT_DATE log -1 --format=%ad)

IF(NOT GIT_REMOTE)
    SET(GIT_REMOTE "LOCAL")
    SET(GIT_REMOTE_URL "file:/${CMAKE_SOURCE_DIR}")
ENDIF()

SET(RELEASE_TAG "${GIT_TAG}")
IF(RELEASE_VERSION)
    STRING(REPLACE "." ";" _version_list "${RELEASE_VERSION}") # 1.2.3 -> 1;2;3
    LIST(GET _version_list 0 RELEASE_MAJOR)
    LIST(GET _version_list 1 RELEASE_MINOR)
    LIST(GET _version_list 2 RELEASE_RELEASE)
ENDIF()

IF(NOT RELEASE_MAJOR OR NOT RELEASE_MINOR OR NOT RELEASE_RELEASE)
    STRING(REPLACE "-" ";" _version_list "${RELEASE_TAG}") # branch-1-2-3 -> branch;1;2;3
    LIST(GET _version_list 1 RELEASE_MAJOR)
    LIST(GET _version_list 2 RELEASE_MINOR)
    LIST(GET _version_list 3 RELEASE_RELEASE)
ENDIF()

IF(NOT RELEASE_MAJOR OR NOT RELEASE_MINOR OR NOT RELEASE_RELEASE)
    SET(RELEASE_MAJOR 0)
    SET(RELEASE_MINOR 0)
    SET(RELEASE_RELEASE 0)
ENDIF()

SET(RELEASE_VERSION "${RELEASE_MAJOR}.${RELEASE_MINOR}.${RELEASE_RELEASE}")
GET_DATE(RELEASE_DATE)

STRING(SUBSTRING "${BRANCH}" 0 12 RELEASE_BRANCH)

IF(NOT RELEASE_BRANCH)
    STRING(SUBSTRING "${GIT_BRANCH}" 0 12 RELEASE_BRANCH)
ENDIF()

IF(NOT RELEASE_BRANCH)
    SET(RELEASE_BRANCH "unknown")
ENDIF()

SET(RELEASE_TAG "${RELEASE_BRANCH}_release-${RELEASE_MAJOR}-${RELEASE_MINOR}-${RELEASE_RELEASE}")

ADD_CUSTOM_TARGET(
    release_tag
    COMMAND ${CMAKE_COMMAND} -E echo "${RELEASE_TAG}"
)

# TODO:
# 	@ \\\$(if \\\$(_git_check_changes),\\\$(info  WARNING: There are uncommitted changes))\\

ADD_CUSTOM_TARGET(
    git_info
    COMMAND ${CMAKE_COMMAND} -E echo " Git repository status: "
    COMMAND ${CMAKE_COMMAND} -E echo " ------------------------------ "
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "   tag: ...... ${GIT_TAG}"
    COMMAND ${CMAKE_COMMAND} -E echo "   branch: ... ${GIT_BRANCH}"
    COMMAND ${CMAKE_COMMAND} -E echo "   commit: ... ${GIT_COMMIT}"
    COMMAND ${CMAKE_COMMAND} -E echo "   remote_url: ${GIT_REMOTE_URL}"
    COMMAND ${CMAKE_COMMAND} -E echo ""
)
