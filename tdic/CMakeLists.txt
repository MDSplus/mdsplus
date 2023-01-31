
###
### TdiShrExt
###

add_library(
    TdiShrExt
    TdiShrExt.c
)

install(TARGETS TdiShrExt)

target_link_libraries(
    TdiShrExt
    PUBLIC
        MdsIpShr
)

add_static_copy(TdiShrExt)

###
### tdic
###

add_executable(
    tdic
    tdic.c
)

install(TARGETS tdic)

target_link_libraries(
    tdic
    PUBLIC
        MdsIpShr
        Readline::Readline
)
