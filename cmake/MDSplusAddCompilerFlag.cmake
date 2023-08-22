include_guard(GLOBAL)

include(MDSplusCheckFlags)

#
# Check if a flag is supported in the C, C++, and Fortran compilers, and add it to CMAKE_{LANG}_FLAGS if it is.
#
macro(mdsplus_add_compiler_flag _flag)

    set(_prefix "_compiler${_flag}")
    string(REPLACE "-" "_" _prefix "${_prefix}")
    string(REPLACE "=" "_" _prefix "${_prefix}")
    string(REPLACE "/" "_" _prefix "${_prefix}")

    mdsplus_check_compiler_flag(${_flag} ${_prefix})

    foreach(_lang IN ITEMS C CXX Fortran)
        if(${_prefix}_${_lang})
            set(CMAKE_${_lang}_FLAGS "${CMAKE_${_lang}_FLAGS} ${_flag}")
        endif()
    endforeach()

endmacro()