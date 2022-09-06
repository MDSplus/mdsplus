
MACRO(ADD_STATIC_COPY _target)

    ADD_LIBRARY(
        "${_target}-static" STATIC
        $<TARGET_OBJECTS:${_target}>
    )

    INSTALL(TARGETS "${_target}-static")

    SET_TARGET_PROPERTIES(
        "${_target}-static"
        PROPERTIES
            OUTPUT_NAME "${_target}"
    )
    
    GET_TARGET_PROPERTY(
        _include_directories
        ${_target} INTERFACE_INCLUDE_DIRECTORIES
    )

    IF(_include_directories)
        TARGET_INCLUDE_DIRECTORIES(
            "${_target}-static"
            PUBLIC
                "${_include_directories}"
        )
    ENDIF()

    GET_TARGET_PROPERTY(
        _compile_definitions
        ${_target} INTERFACE_COMPILE_DEFINITIONS
    )

    IF(_compile_definitions)
        TARGET_COMPILE_DEFINITIONS(
            "${_target}-static"
            PUBLIC
                ${_compile_definitions}
        )
    ENDIF()

    GET_TARGET_PROPERTY(
        _compile_options
        ${_target} INTERFACE_COMPILE_OPTIONS
    )

    IF(_compile_options)
        TARGET_COMPILE_DEFINITIONS(
            "${_target}-static"
            PUBLIC
                ${_compile_options}
        )
    ENDIF()

    GET_TARGET_PROPERTY(
        _link_libraries
        ${_target} INTERFACE_LINK_LIBRARIES
    )

    IF(_link_libraries)
        TARGET_LINK_LIBRARIES(
            "${_target}-static"
            PUBLIC
                ${_link_libraries}
        )
    ENDIF()

    GET_TARGET_PROPERTY(
        _link_options
        ${_target} INTERFACE_LINK_OPTIONS
    )

    IF(_link_options)
        TARGET_LINK_OPTIONS(
            "${_target}-static"
            PUBLIC
                ${_link_options}
        )
    ENDIF()

ENDMACRO()