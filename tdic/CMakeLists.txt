
###
### TdiShrExt
###

add_library(
    TdiShrExt
    TdiShrExt.c
)

target_link_libraries(
    TdiShrExt
    PUBLIC
        MdsIpShr
)

mdsplus_add_static_copy(TdiShrExt _static_target)

install(TARGETS TdiShrExt ${_static_target})

###
### tdic
###

add_executable(
    tdic
    tdic.c
)

target_link_libraries(
    tdic
    PUBLIC
        MdsIpShr
        Readline::Readline
)

install(TARGETS tdic)
