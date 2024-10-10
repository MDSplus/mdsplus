include_guard(GLOBAL)

# The BUILD_TESTING option is created by include(CTest), so we just add it to the list manually
set(MDSPLUS_OPTION_LIST "BUILD_TESTING")

#
# mdsplus_option(<name> <type> <description>
#                [DEFAULT <default>]
#                [OPTIONS <option>...]
#                [FORCE])
#
# Call set(<name> <default> CACHE <type> <description>) to create a persistent option.
# If OPTIONS is set, then the STRINGS property of the option is set to allow for 
# multiple-choice selections in ccmake or cmake-gui.
#
function(mdsplus_option _name _type _description)

    cmake_parse_arguments(
        PARSE_ARGV 3 ARGS
        # Booleans
        "FORCE"
        # Single-Value
        "DEFAULT"
        # Multi-Value
        "OPTIONS"
    )

    if(ARGS_FORCE)
        set(_force FORCE)
    endif()

    if(DEFINED ARGS_OPTIONS)
        string(REPLACE ";" ", " _options "${ARGS_OPTIONS}")
        set(_description "${_description} Options are: ${_options}")
    endif()

    if(DEFINED ARGS_DEFAULT)
        set(_description "${_description} [default: ${ARGS_DEFAULT}]")
    else()
        set(ARGS_DEFAULT "")
    endif()

    set(${_name}
        ${ARGS_DEFAULT}
        CACHE ${_type}
        "${_description}"
        ${_force}
    )

    if(ARGS_OPTIONS)
        set_property(
            CACHE ${_name}
            PROPERTY STRINGS ${ARGS_OPTIONS}
        )
    endif()

    list(APPEND MDSPLUS_OPTION_LIST "${_name}")
    set(MDSPLUS_OPTION_LIST "${MDSPLUS_OPTION_LIST}" PARENT_SCOPE)

endfunction()

function(mdsplus_print_options)

    message(STATUS "Configuration Options:")
    list(APPEND CMAKE_MESSAGE_INDENT "    ")

    list(SORT MDSPLUS_OPTION_LIST)

    foreach(_name IN LISTS MDSPLUS_OPTION_LIST)
        message(STATUS "${_name}: ${${_name}}")
    endforeach()

    list(POP_BACK CMAKE_MESSAGE_INDENT)

endfunction()