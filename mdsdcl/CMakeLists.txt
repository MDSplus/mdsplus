
###
### Mdsdcl
###

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/mdsdclVersionInfo.c.in
    ${CMAKE_CURRENT_BINARY_DIR}/mdsdclVersionInfo.c
)

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
