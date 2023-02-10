
###
### RemCamShr / CamShr
###

if(TARGET CamShr)
    set(_target "RemCamShr")
else()
    set(_target "CamShr")
endif()

add_library(
    ${_target}
    CamIosb.c
    CamMulti.c
    CamSingle.c
)

target_link_libraries(
    ${_target}
    PUBLIC
        MdsIpShr
)

install(TARGETS ${_target})