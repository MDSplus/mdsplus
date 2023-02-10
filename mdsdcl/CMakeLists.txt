
###
### Mdsdcl
###

# Generated Sources

configure_file(
    mdsdclVersionInfo.c.in
    ${CMAKE_CURRENT_BINARY_DIR}/mdsdclVersionInfo.c
)

set(_generated_sources
    cmdParse.c
    cmdParse.output
    cmdParseLex.c
    dcllex.h
    mdsdclDeltatimeToSeconds.c
    ocldToXml.c
)

set(_run_yylex_gen FALSE)
foreach(_filename IN LISTS _generated_sources)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_filename}")
        message("Missing mdsdcl/${_filename}, will run deploy/yylex.gen mdsdcl")
        set(_run_yylex_gen TRUE)
    endif()
endforeach()

if(_run_yylex_gen)
    # TODO: Convert yylex.gen to python
    message("Running deploy/yylex.gen mdsdcl")
    execute_process(
        COMMAND deploy/yylex.gen mdsdcl
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

add_library(
    Mdsdcl
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

target_include_directories(
    Mdsdcl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    Mdsdcl
    PUBLIC
        MdsIpShr
        Readline::Readline
)

target_compile_options(
    Mdsdcl
    PRIVATE
        -Wno-sign-compare
        -Wno-unused-parameter
)

target_compile_definitions(
    Mdsdcl
    PRIVATE
        -DLIBPREFIX=Mdsdcl
)

install(TARGETS Mdsdcl)

# set_source_files_properties(
#     cmdParseLex.c
#     PROPERTIES
#         COMPILE_OPTIONS "-Wno-sign-compare;-Wno-unused-parameter"
# )

# set_source_files_properties(
#     mdsdclDeltatimeToSeconds.c
#     PROPERTIES
#         COMPILE_OPTIONS "-Wno-sign-compare"
# )

###
### mdsdcl
###

add_executable(
    mdsdcl
    mdsdcl.c
)

target_link_libraries(
    mdsdcl
    PUBLIC
        Mdsdcl
)

install(TARGETS mdsdcl)