include_guard(GLOBAL)

include(CheckCompilerFlag)

#
# mdsplus_check_add_flags(
#                     [COMPILER <flags>]
#                     [LINKER <flags>]
#                     [LANGUAGES <langs>])
#
# Check if the COMPILER/LINKER flags are supported for the given LANGUAGES, and add them if they are.
# The compiler flags will be added to CMAKE_${LANG}_FLAGS for the supported languages, and the linker flags will be passed
# to add_link_options() if all languages pass their checks.
# LANGUAGES will default to "C;CXX;Fortran" if not specified.
#
function(mdsplus_check_add_flags)

    # The ARGS is a prefix to all parsed argument variables
    cmake_parse_arguments(
        PARSE_ARGV 0 ARGS
        # Booleans
        ""
        # Single-Value
        ""
        # Multi-Value
        "COMPILER;LINKER;LANGUAGES"
    )

    if(NOT DEFINED ARGS_LANGUAGES)
        set(ARGS_LANGUAGES C CXX Fortran)
    endif()

    set(_all_passed TRUE)
    foreach(_lang IN ITEMS ${ARGS_LANGUAGES})

        # This will be the name of a cache variable that will persist between configures, so it needs to
        # be unique and consistent, computed from the requested flags
        set(_variable_name "MDSPLUS_CHECK_ADD_FLAGS_${_lang}_${ARGS_COMPILER}_${ARGS_LINKER}")
        string(REPLACE ";" "_" _variable_name "${_variable_name}")
        string(REPLACE "-" "_" _variable_name "${_variable_name}")
        string(REPLACE "/" "_" _variable_name "${_variable_name}")
        string(REPLACE "=" "_" _variable_name "${_variable_name}")

        if(NOT DEFINED ${_variable_name})
            message(CHECK_START "Checking if flags are supported for ${_lang}")

            if(DEFINED ARGS_COMPILER)

                string(REPLACE ";" " " _compiler_flags_message "${ARGS_COMPILER}")

                if(_lang STREQUAL "C")
                    set(_compiler_flags_message "CFLAGS=${_compiler_flags_message}")
                elseif(_lang STREQUAL "CXX")
                    set(_compiler_flags_message "CXXFLAGS=${_compiler_flags_message}")
                elseif(_lang STREQUAL "Fortran")
                    set(_compiler_flags_message "FCFLAGS=${_compiler_flags_message}")
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
                        set(_result ${${_variable_name}})

                    unset(CMAKE_REQUIRED_FLAGS)
                    unset(CMAKE_REQUIRED_LINK_OPTIONS)

                set(CMAKE_REQUIRED_QUIET OFF)

            set(CMAKE_${_lang}_FLAGS "${_saved_flags}")
            
            if(${_result})
                message(CHECK_PASS "Success")
            else()
                message(CHECK_FAIL "Failed")
            endif()
        
        endif()

        if(${_variable_name})
            if(DEFINED ARGS_COMPILER)
                set(CMAKE_${_lang}_FLAGS "${CMAKE_${_lang}_FLAGS} ${ARGS_COMPILER}" PARENT_SCOPE)
            endif()
        else()
            set(_all_passed FALSE)
        endif()
        
    endforeach()

    if(DEFINED ARGS_LINKER AND ${_all_passed})
        add_link_options(${ARGS_LINKER})
    endif()

endfunction()