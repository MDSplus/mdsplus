
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

find_package(BISON)
find_package(FLEX)
BISON_TARGET(
    cmdParse
    cmdParse.y
    ${CMAKE_CURRENT_BINARY_DIR}/cmdParse.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/dclyacc.h
)
FLEX_TARGET(
    cmdParseLex
    cmdParse.l
    ${CMAKE_CURRENT_BINARY_DIR}/cmdParseLex.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/dcllex.h
)
FLEX_TARGET(
    mdsdclDeltatimeToSeconds
    mdsdclDeltatimeToSeconds.l
    ${CMAKE_CURRENT_BINARY_DIR}/mdsdclDeltatimeToSeconds.c
)
FLEX_TARGET(
    ocldToXml
    ocldToXml.l
    ${CMAKE_CURRENT_BINARY_DIR}/ocldToXml.c
)
ADD_FLEX_BISON_DEPENDENCY(cmdParseLex cmdParse)
# add_dependencies(Mdsdcl cmdParse cmdParseLex mdsdclDeltatimeToSeconds ocldToXml)

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