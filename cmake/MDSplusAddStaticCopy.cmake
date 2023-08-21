#
# Create a static copy of a shared library, with all the same properties.
# Note: If BUILD_SHARED_LIBS=OFF, then this creates an ALIAS target instead.
#
macro(mdsplus_add_static_copy _target)

    if(BUILD_SHARED_LIBS)

        add_library(
            "${_target}-static" STATIC
            $<TARGET_OBJECTS:${_target}>
        )

        set_target_properties(
            "${_target}-static"
            PROPERTIES
                OUTPUT_NAME "${_target}"
        )
        
        get_target_property(
            _include_directories
            ${_target} INTERFACE_INCLUDE_DIRECTORIES
        )

        if(_include_directories)
            target_include_directories(
                "${_target}-static"
                PUBLIC
                    "${_include_directories}"
            )
        endif()

        get_target_property(
            _compile_definitions
            ${_target} INTERFACE_COMPILE_DEFINITIONS
        )

        if(_compile_definitions)
            target_compile_definitions(
                "${_target}-static"
                PUBLIC
                    ${_compile_definitions}
            )
        endif()

        get_target_property(
            _compile_options
            ${_target} INTERFACE_COMPILE_OPTIONS
        )

        if(_compile_options)
            target_compile_definitions(
                "${_target}-static"
                PUBLIC
                    ${_compile_options}
            )
        endif()

        get_target_property(
            _link_libraries
            ${_target} INTERFACE_LINK_LIBRARIES
        )

        if(_link_libraries)
            target_link_libraries(
                "${_target}-static"
                PUBLIC
                    ${_link_libraries}
            )
        endif()

        get_target_property(
            _link_options
            ${_target} INTERFACE_LINK_OPTIONS
        )

        if(_link_options)
            target_link_options(
                "${_target}-static"
                PUBLIC
                    ${_link_options}
            )
        endif()

    else()

        add_library("${_target}-static" ALIAS ${_target})

    endif()

endmacro()