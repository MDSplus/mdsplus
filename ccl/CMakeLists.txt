
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
        Mdsdcl
        CamShr
)

install(TARGETS ccl_commands)

###
### Scripts
###

if(WIN32)
    set(_ext ".bat")
endif()

install(
    FILES mdsccl${_ext}
    PERMISSIONS ${MODE_755}
    TYPE BIN
)