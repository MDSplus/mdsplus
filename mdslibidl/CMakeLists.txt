
###
### MdsLibIdl
###

add_library(
    MdsLibIdl
    MdsLibIdl.c
)

install(TARGETS MdsLibIdl)

target_link_libraries(
    MdsLibIdl
    PUBLIC
        TdiShr
)
