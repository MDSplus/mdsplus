# FindValgrind.cmake
#
# Finds Valgrind and the available tools
#
# This will define the following variables
#
#   Valgrind_FOUND
#   Valgrind_memcheck_FOUND
#   Valgrind_helgrind_FOUND
#   Valgrind_drd_FOUND
#   Valgrind_exp_sgcheck_FOUND
#   Valgrind_TOOL_LIST
#   Valgrind_EXECUTABLE
#   Valgrind_VERSION
#   HAVE_VALGRIND_H
#   HAVE_VALGRIND_MEMCHECK_H
#
# The following variables can be set as arguments
#
#   Valgrind_ROOT_DIR
#   Valgrind_REQUESTED_TOOL_LIST
#

find_program(
    Valgrind_EXECUTABLE
    NAMES valgrind
    PATHS ${Valgrind_ROOT_DIR}
    PATH_SUFFIXES bin
)

if(Valgrind_EXECUTABLE)

    if(NOT EXISTS Valgrind_VERSION)

        execute_process(
            COMMAND ${Valgrind_EXECUTABLE} --version
            OUTPUT_VARIABLE Valgrind_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        # Remove any text that is not part of the version, e.g. "valgrind-1.2.3" -> "1.2.3"
        string(REGEX REPLACE "[\-a-zA-Z_]" "" Valgrind_VERSION "${Valgrind_VERSION}")

    endif()

    if(NOT EXISTS Valgrind_TOOL_LIST)

        foreach(_tool IN LISTS Valgrind_REQUESTED_TOOL_LIST)

            execute_process(
                COMMAND ${Valgrind_EXECUTABLE} --tool=${_tool} --help
                OUTPUT_QUIET
                ERROR_QUIET
                RESULT_VARIABLE _result
            )

            if(_result EQUAL 0)
                set(_found TRUE)

                list(APPEND Valgrind_TOOL_LIST
                    ${_tool}
                )
            else()
                set(_found FALSE)
                message(STATUS "Unable to find valgrind --tool ${_tool}")
            endif()


            string(REPLACE "-" "_" _tool_no_dash ${_tool})
            set(Valgrind_${_tool_no_dash}_FOUND ${_found})

        endforeach()

    endif()

endif()

include(CheckIncludeFiles)

check_include_files("valgrind/valgrind.h" HAVE_VALGRIND_H)
check_include_files("valgrind/memcheck.h" HAVE_VALGRIND_MEMCHECK_H)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Valgrind
    VERSION_VAR Valgrind_VERSION
    REQUIRED_VARS # The first one is displayed in the message
        Valgrind_EXECUTABLE
)

mark_as_advanced(
    Valgrind_EXECUTABLE
    Valgrind_TOOL_LIST
)
