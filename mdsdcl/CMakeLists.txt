
###
### Mdsdcl
###

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/mdsdclVersionInfo.c.in
    ${CMAKE_CURRENT_SOURCE_DIR}/mdsdclVersionInfo.c
)

add_library(
    Mdsdcl
    mdsdclThreadStatic.c
    cmdExecute.c
    cmdHelp.c
    mdsdcl_commands.c
    mdsdclVersionInfo.c
    mdsdcl_show_version.c
    cmdParse.c
    cmdParseLex.c
    mdsdclDeltatimeToSeconds.c
    mdsdclVersionInfo.c
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



# if(WIN32)
#     set(_ext ".exe")
# endif()

# execute_process(
#     COMMAND ${Python_EXECUTABLE}
#         ${CMAKE_SOURCE_DIR}/scripts/generate-launch-targets.py
#         --name "mdsdcl"
#         --executable "${CMAKE_BINARY_DIR}/bin/mdsdcl${_ext}"
#         --working-dir "${CMAKE_SOURCE_DIR}"
#         --runtime-path "${CMAKE_BINARY_DIR}/lib"
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )