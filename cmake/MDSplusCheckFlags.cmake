include_guard(GLOBAL)

include(CheckCompilerFlag)
include(CheckLinkerFlag)

#
# Call check_compiler_flag() for C, CXX, and Fortran, then set ${_prefix}_{LANG} for each one.
# This also has better messaging than check_compiler_flag()
#
macro(mdsplus_check_compiler_flag _flag _prefix)

    foreach(_lang IN ITEMS C CXX Fortran)
        set(_variable_name "${_prefix}_${_lang}")
        if(NOT DEFINED ${_variable_name})

            message(CHECK_START "Checking if the ${_lang} compiler supports ${_flag}")

            set(CMAKE_REQUIRED_QUIET ON)
            check_compiler_flag(${_lang} ${_flag} ${_variable_name})
            set(CMAKE_REQUIRED_QUIET OFF)
            
            if(${_variable_name})
                message(CHECK_PASS "Success")
            else()
                message(CHECK_FAIL "Failed")
            endif()

        endif()
    endforeach()

endmacro()

#
# Call check_linker_flag() for C, CXX, and Fortran, then set ${_prefix}_{LANG} for each one.
# This also has better messaging than check_linker_flag()
#
macro(mdsplus_check_linker_flag _flag _prefix)

    foreach(_lang IN ITEMS C CXX Fortran)
        set(_variable_name "${_prefix}_${_lang}")
        if(NOT DEFINED ${_variable_name})

            message(CHECK_START "Checking if the ${_lang} linker supports ${_flag}")

            set(CMAKE_REQUIRED_QUIET ON)
            check_linker_flag(${_lang} ${_flag} ${_variable_name})
            set(CMAKE_REQUIRED_QUIET OFF)
            
            if(${_variable_name})
                message(CHECK_PASS "Success")
            else()
                message(CHECK_FAIL "Failed")
            endif()

        endif()
    endforeach()

endmacro()