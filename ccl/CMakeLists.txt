
###
### ccl_commands
###

add_library(
    ccl_commands
    ccl_verbs.c
)

target_link_libraries(
    ccl_commands
    PUBLIC
        libMdsdcl
        CamShr
)

install(TARGETS ccl_commands)

###
### Scripts
###

if(WIN32)
    set(_ext ".bat")
endif()

set(_script_list
    mdsccl${_ext}
)

file(COPY ${_script_list}
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

install(
    FILES ${_script_list}
    TYPE BIN
    PERMISSIONS ${MODE_755}
)