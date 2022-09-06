
###
### MdsLibIdl
###

ADD_LIBRARY(
    MdsLibIdl SHARED
    MdsLibIdl.c
)

INSTALL(TARGETS MdsLibIdl)

TARGET_LINK_LIBRARIES(
    MdsLibIdl
    PUBLIC
        TdiShr
)
