include_guard(GLOBAL)

#
# mdsplus_generate_files(COMMAND <command> [<arg>...])
#                        GENERATED_FILES <files>)
#
# Check if any of the GENERATED_FILES are missing, and run COMMAND to generate them if they are.
#
function(mdsplus_generate_files)

    cmake_parse_arguments(
        PARSE_ARGV 0 ARGS
        # Booleans
        ""
        # Single-Value
        ""
        # Multi-Value
        "COMMAND;GENERATED_FILES"
    )

    string(REPLACE ";" " " _print_command "${ARGS_COMMAND}")

    # Check to see if we already have all the files generated
    set(_will_run_command OFF)
    foreach(_file IN LISTS ARGS_GENERATED_FILES)
        if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${_file})
            message("Missing '${_file}', will run ${_print_command}")
            set(_will_run_command TRUE)
        endif()
    endforeach()

    # If not, generate them
    if(_will_run_command)
        message(STATUS "Running ${_print_command}")
        execute_process(
            COMMAND ${ARGS_COMMAND}
            RESULT_VARIABLE _result
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

        if(NOT _result EQUAL 0)
            message(FATAL_ERROR "Failed to run ${_print_command}")
        endif()
    endif()

endfunction()
