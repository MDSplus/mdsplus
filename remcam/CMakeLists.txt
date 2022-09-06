
###
### RemCamShr / CamShr
###

IF(TARGET CamShr)
    SET(_target "RemCamShr")
ELSE()
    SET(_target "CamShr")
ENDIF()

ADD_LIBRARY(
    ${_target} SHARED
    CamIosb.c
    CamMulti.c
    CamSingle.c
)

INSTALL(TARGETS ${_target})

TARGET_LINK_LIBRARIES(
    ${_target}
    PUBLIC
        MdsIpShr
)