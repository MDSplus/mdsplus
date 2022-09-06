
###
### ccl_commands
###

ADD_LIBRARY(
    ccl_commands SHARED
    ccl_verbs.c
)

TARGET_LINK_LIBRARIES(
    ccl_commands
    PUBLIC
        Mdsdcl
        CamShr
)

INSTALL(TARGETS ccl_commands)

IF(WIN32)
    SET(_ext ".bat")
ENDIF()

INSTALL(
    FILES mdsccl${_ext}
    PERMISSIONS ${MODE_755}
    TYPE BIN
)