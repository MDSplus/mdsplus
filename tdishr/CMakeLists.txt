
###
### TdiShr
###

# Generated Sources

SET(
    _generated_sources
    TdiYacc.c
    tdiyacc.h
    TdiYacc.output
    TdiHash.c
    TdiLex.c
    tdilex.h
)

SET(_run_yylex_gen FALSE)
FOREACH(_filename IN LISTS _generated_sources)
    IF(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_filename}")
        MESSAGE("Missing mdsdcl/${_filename}, will run deploy/yylex.gen tdishr")
        SET(_run_yylex_gen TRUE)
    ENDIF()
ENDFOREACH()

IF(_run_yylex_gen)
    MESSAGE("Running deploy/yylex.gen tdishr")
    # TODO: Convert yylex.gen to python
    EXECUTE_PROCESS(
        COMMAND deploy/yylex.gen tdishr
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
ENDIF()

FILE(GLOB
    _source_list
    "*.c"
)

ADD_LIBRARY(
    TdiShr SHARED
    ${_source_list}
)

INSTALL(TARGETS TdiShr)

TARGET_LINK_LIBRARIES(
    TdiShr
    PUBLIC
        TreeShr
)

ADD_STATIC_COPY(TdiShr)