include_guard(GLOBAL)

include(CheckCompilerFlag)

#
# mdsplus_check_flags(<prefix>
#                     [COMPILER <flags>]
#                     [LINKER <flags>])
#
# Check if the combination of COMPILER and LINKER flags for C, CXX, and Fortran works, and sets ${_prefix}_{LANG} for each one.
#
function(mdsplus_check_flags _prefix)

    cmake_parse_arguments(
        PARSE_ARGV 1 ARGS
        # Booleans
        ""
        # Single-Value
        ""
        # Multi-Value
        "COMPILER;LINKER"
    )

    foreach(_lang IN ITEMS C CXX Fortran)
        set(_variable_name "${_prefix}_${_lang}")

        if(NOT DEFINED ${_variable_name})
            
            message(CHECK_START "Checking if flags are supported for ${_lang}")

            if(DEFINED ARGS_COMPILER)

                string(REPLACE ";" " " _compiler_flags_message "${ARGS_COMPILER}")

                if(_lang STREQUAL "C")
                    set(_compiler_flags_message "CFLAGS=${_compiler_flags_message}")
                elseif(_lang STREQUAL "CXX")
                    set(_compiler_flags_message "CXXFLAGS=${_compiler_flags_message}")
                elseif(_lang STREQUAL "Fortran")
                    set(_compiler_flags_message "FCFLAGS=${_compiler_flags_message}") # TODO: Use FFLAGS instead?
                endif()

                message(STATUS "    ${_compiler_flags_message}")

            endif()

            if(DEFINED ARGS_LINKER)
            
                string(REPLACE ";" " " _linker_flags_message "${ARGS_LINKER}")
                set(_linker_flags_message "LDFLAGS=${_linker_flags_message}")

                message(STATUS "    ${_linker_flags_message}")

            endif()

            # Remove the current flags so they don't interfere with the test
            set(_saved_flags "${CMAKE_${_lang}_FLAGS}")
            unset(CMAKE_${_lang}_FLAGS)

                # Disable the default messaging to replace it with our own
                set(CMAKE_REQUIRED_QUIET ON)

                    # In order to check more than one flag at a time, we use these and set <flag> to ""
                    set(CMAKE_REQUIRED_FLAGS ${ARGS_COMPILER})
                    set(CMAKE_REQUIRED_LINK_OPTIONS ${ARGS_LINKER})

                        check_compiler_flag(${_lang} "" ${_variable_name})
                        set(${_variable_name} PARENT_SCOPE)

                    unset(CMAKE_REQUIRED_FLAGS)
                    unset(CMAKE_REQUIRED_LINK_OPTIONS)

                set(CMAKE_REQUIRED_QUIET OFF)

            set(CMAKE_${_lang}_FLAGS "${_saved_flags}")
            
            if(${_variable_name})
                message(CHECK_PASS "Success")
            else()
                message(CHECK_FAIL "Failed")
            endif()

        endif()
    endforeach()

endfunction()