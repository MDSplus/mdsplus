include_guard(GLOBAL)

find_package(Git)

if(GIT_FOUND)

    # Convenience macro for all the git queries we need to make
    macro(mdsplus_git _output_variable) # additional arguments are in ${ARGN}
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

endif()
