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
#   HAVE_VALGRIND_H
#   HAVE_VALGRIND_MEMCHECK_H
#
# The following variables can be set as arguments
#
#   Valgrind_ROOT_DIR
#

find_program(
    Valgrind_EXECUTABLE
    NAMES valgrind
    PATHS ${Valgrind_ROOT_DIR}
    PATH_SUFFIXES bin
)

if(Valgrind_EXECUTABLE)

    set(_tool_list
        memcheck
        helgrind
        drd
        exp-sgcheck
    )

    unset(Valgrind_TOOL_LIST)

    foreach(_tool IN LISTS _tool_list)

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
        endif()


        string(REPLACE "-" "_" _tool_no_dash ${_tool})
        set(Valgrind_${_tool_no_dash}_FOUND ${_found})

    endforeach()

endif()

include(CheckIncludeFiles)

check_include_files("valgrind/valgrind.h" HAVE_VALGRIND_H)
check_include_files("valgrind/memcheck.h" HAVE_VALGRIND_MEMCHECK_H)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Valgrind
    REQUIRED_VARS # The first one is displayed in the message
        Valgrind_EXECUTABLE
)

mark_as_advanced(
    Valgrind_EXECUTABLE
)
