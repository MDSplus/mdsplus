
###
### TdiShrExt
###

ADD_LIBRARY(
    TdiShrExt SHARED
    TdiShrExt.c
)

INSTALL(TARGETS TdiShrExt)

TARGET_LINK_LIBRARIES(
    TdiShrExt
    PUBLIC
        MdsIpShr
)

ADD_STATIC_COPY(TdiShrExt)

###
### tdic
###

ADD_EXECUTABLE(
    tdic
    tdic.c
)

INSTALL(TARGETS tdic)

TARGET_LINK_LIBRARIES(
    tdic
    PUBLIC
        MdsIpShr
        Readline::Readline
)
