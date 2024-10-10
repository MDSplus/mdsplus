
###
### libMdsdcl
###

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/mdsdclVersionInfo.c.in
    ${CMAKE_CURRENT_BINARY_DIR}/mdsdclVersionInfo.c
)

add_library(
    libMdsdcl
    mdsdclThreadStatic.c
    cmdExecute.c
    cmdHelp.c
    mdsdcl_commands.c
    mdsdclVersionInfo.c
    mdsdcl_show_version.c
    cmdParse.c
    cmdParseLex.c
    mdsdclDeltatimeToSeconds.c
    ${CMAKE_CURRENT_BINARY_DIR}/mdsdclVersionInfo.c
)

# On case-insensitive filesystems, we can't have both mdstcl and Mdstcl as targets
set_target_properties(
    libMdsdcl
    PROPERTIES
        OUTPUT_NAME Mdsdcl
)

target_include_directories(
    libMdsdcl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
    libMdsdcl
    PUBLIC
        MdsIpShr
        Readline::Readline
)

target_compile_options(
    libMdsdcl
    PRIVATE
        -Wno-sign-compare
        -Wno-unused-parameter
)

target_compile_definitions(
    libMdsdcl
    PRIVATE
        -DLIBPREFIX=Mdsdcl
)

install(TARGETS libMdsdcl)

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
        libMdsdcl
)

install(TARGETS mdsdcl)
