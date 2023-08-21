#
# Check if any of the GENERATED_FILES are missing, and if so, run COMMAND to generate them. 
#
# mdsplus_generate_files(COMMAND <command> [<arg>...])
#                        GENERATED_FILES <files>)
#
macro(mdsplus_generate_files)

    set(_boolean_options)
    set(_single_value_options)
    set(_multi_value_options COMMAND GENERATED_FILES)

    cmake_parse_arguments(
        _run_if_missing
        "${_boolean_options}"
        "${_single_value_options}"
        "${_multi_value_options}"
        ${ARGN}
    )

    string(REPLACE ";" " " _print_command "${_run_if_missing_COMMAND}")

    # Check to see if we already have all the files generated
    set(_will_run_command OFF)
    foreach(_file IN LISTS _run_if_missing_GENERATED_FILES)
        if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${_file})
            message("Missing '${_file}', will run ${_print_command}")
            set(_will_run_command TRUE)
        endif()
    endforeach()

    # If not, generate them
    if(_will_run_command)
        message(STATUS "Running ${_print_command}")
        execute_process(
            COMMAND ${_run_if_missing_COMMAND}
            RESULT_VARIABLE _result
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

        if(NOT _result EQUAL 0)
            message(FATAL_ERROR "Failed to run ${_print_command}")
        endif()
    endif()

endmacro()
