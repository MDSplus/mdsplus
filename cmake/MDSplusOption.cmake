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

    # The 3 means to skip the first three arguments: _name, _type, and, _description
    # The ARGS is a prefix to all parsed argument variables
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

    # Determine the length of the longest option name
    set(_max_option_name_length 0)
    foreach(_name IN LISTS MDSPLUS_OPTION_LIST)
        string(LENGTH "${_name}" _option_name_length)
        if(_option_name_length GREATER _max_option_name_length)
            set(_max_option_name_length ${_option_name_length})
        endif()
    endforeach()

    list(SORT MDSPLUS_OPTION_LIST)

    foreach(_name IN LISTS MDSPLUS_OPTION_LIST)
        string(LENGTH "${_name}" _option_name_length)

        # Apply padding so that all the values are aligned in one column
        set(_padding "")
        while(_option_name_length LESS _max_option_name_length)
            set(_padding "${_padding} ")
            math(EXPR _option_name_length "${_option_name_length} + 1")
        endwhile()
        
        message(STATUS "${_name}:${_padding} ${${_name}}")
    endforeach()

    list(POP_BACK CMAKE_MESSAGE_INDENT)

endfunction()