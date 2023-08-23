include_guard(GLOBAL)

include(MDSplusCheckFlags)

#
# mdsplus_add_compiler_flags(<flags>)
#
# Check if the flags are supported in the C, CXX, and Fortran compilers, and add it to CMAKE_{LANG}_FLAGS if it is.
#
function(mdsplus_add_compiler_flags _flags)

    string(REPLACE ";" " " _flags "${_flags}")

    set(_prefix "_compiler${_flags}")
    string(REPLACE " " "_" _prefix "${_prefix}")
    string(REPLACE "-" "_" _prefix "${_prefix}")
    string(REPLACE "=" "_" _prefix "${_prefix}")
    string(REPLACE "/" "_" _prefix "${_prefix}")

    mdsplus_check_flags(${_prefix}
        COMPILER ${_flags}
    )

    foreach(_lang IN ITEMS C CXX Fortran)
        if(${_prefix}_${_lang})
            set(CMAKE_${_lang}_FLAGS "${CMAKE_${_lang}_FLAGS} ${_flags}" PARENT_SCOPE)
        endif()
    endforeach()

endfunction()