
###
### TreeShr
###

# Generated Sources

SET(
    _generated_sources
    TreeFindNodeWild.c
)

SET(_run_yylex_gen FALSE)
FOREACH(_filename IN LISTS _generated_sources)
    IF(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_filename}")
        MESSAGE("Missing mdsdcl/${_filename}, will run deploy/yylex.gen treeshr")
        SET(_run_yylex_gen TRUE)
    ENDIF()
ENDFOREACH()

IF(_run_yylex_gen)
    MESSAGE("Running deploy/yylex.gen treeshr")
    # TODO: Convert yylex.gen to python
    EXECUTE_PROCESS(
        COMMAND deploy/yylex.gen treeshr
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
ENDIF()

FILE(GLOB
    _source_list
    "*.c"
)

ADD_LIBRARY(
    TreeShr SHARED
    ${_source_list}
)

INSTALL(TARGETS TreeShr)

TARGET_LINK_LIBRARIES(
    TreeShr
    PUBLIC
        MdsShr
)

ADD_STATIC_COPY(TreeShr)