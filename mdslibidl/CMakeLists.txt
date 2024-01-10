
###
### MdsLibIdl
###

add_library(
    MdsLibIdl
    MdsLibIdl.c
)

target_link_libraries(
    MdsLibIdl
    PUBLIC
        TdiShr
)

install(TARGETS MdsLibIdl)
