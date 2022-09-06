
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

add_static_copy(TdiShrExt)

install(TARGETS TdiShrExt TdiShrExt-static)

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
