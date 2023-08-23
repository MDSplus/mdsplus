include_guard(GLOBAL)

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
        set(_description "${_description}, options are: ${_options}")
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

endfunction()