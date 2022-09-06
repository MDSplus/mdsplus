
###
### Mdsdcl
###

# Generated Sources

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/mdsdclVersionInfo.c.in
    ${CMAKE_CURRENT_BINARY_DIR}/mdsdclVersionInfo.c
)

SET(
    _generated_sources
    cmdParse.c
    cmdParse.output
    cmdParseLex.c
    dcllex.h
    mdsdclDeltatimeToSeconds.c
    ocldToXml.c
)

SET(_run_yylex_gen FALSE)
FOREACH(_filename IN LISTS _generated_sources)
    IF(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_filename}")
        MESSAGE("Missing mdsdcl/${_filename}, will run deploy/yylex.gen mdsdcl")
        SET(_run_yylex_gen TRUE)
    ENDIF()
ENDFOREACH()

IF(_run_yylex_gen)
    MESSAGE("Running deploy/yylex.gen mdsdcl")
    # TODO: Convert yylex.gen to python
    EXECUTE_PROCESS(
        COMMAND deploy/yylex.gen mdsdcl
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
ENDIF()

ADD_LIBRARY(
    Mdsdcl SHARED
    mdsdclThreadStatic.c
    cmdExecute.c
    cmdHelp.c
    cmdParseLex.c
    cmdParse.c
    mdsdclDeltatimeToSeconds.c
    mdsdcl_commands.c
    mdsdclVersionInfo.c
    mdsdcl_show_version.c
    ${CMAKE_CURRENT_BINARY_DIR}/mdsdclVersionInfo.c
)

INSTALL(TARGETS Mdsdcl)

TARGET_INCLUDE_DIRECTORIES(
    Mdsdcl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

TARGET_LINK_LIBRARIES(
    Mdsdcl
    PUBLIC
        MdsIpShr
        Readline::Readline
)

###
### mdsdcl
###

ADD_EXECUTABLE(
    mdsdcl
    mdsdcl.c
)

INSTALL(TARGETS mdsdcl)

TARGET_LINK_LIBRARIES(
    mdsdcl
    PUBLIC
        Mdsdcl
)