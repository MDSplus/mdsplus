include_guard(GLOBAL)

#
# mdsplus_add_static_copy(<target> <out_static_target>)
#
# Create a static copy of a shared library, with all the same properties,
# and set ${out_static_target} to the name of the static library target.
# Note: If BUILD_SHARED_LIBS=OFF, then ${out_static_target} will be unset.
#
macro(mdsplus_add_static_copy _target _out_static_target)

    set(_static_target "${_target}-static")

    if(BUILD_SHARED_LIBS)

        set(${_out_static_target} ${_static_target})

        add_library(
            ${_static_target} STATIC
            $<TARGET_OBJECTS:${_target}>
        )

        set_target_properties(
            ${_static_target}
            PROPERTIES
                OUTPUT_NAME "${_target}"
        )
        
        get_target_property(
            _include_directories
            ${_target} INTERFACE_INCLUDE_DIRECTORIES
        )

        if(_include_directories)
            target_include_directories(
                ${_static_target}
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
                ${_static_target}
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
                ${_static_target}
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
                ${_static_target}
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
                ${_static_target}
                PUBLIC
                    ${_link_options}
            )
        endif()

    else()

        unset(${_out_static_target})

        # Add an alias for things that link against the static target directly
        add_library(${_static_target} ALIAS ${_target})

    endif()

endmacro()