
###
### Mdsdcl
###

configure_file(
    mdsdclVersionInfo.c.in
    ${CMAKE_CURRENT_BINARY_DIR}/mdsdclVersionInfo.c
)

add_library(
    Mdsdcl
    mdsdclThreadStatic.c
    cmdExecute.c
    cmdHelp.c
    mdsdcl_commands.c
    mdsdclVersionInfo.c
    mdsdcl_show_version.c
    ${CMAKE_CURRENT_BINARY_DIR}/cmdParse.c
    ${CMAKE_CURRENT_BINARY_DIR}/cmdParseLex.c
    ${CMAKE_CURRENT_BINARY_DIR}/mdsdclDeltatimeToSeconds.c
    ${CMAKE_CURRENT_BINARY_DIR}/mdsdclVersionInfo.c
)

target_include_directories(
    Mdsdcl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
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