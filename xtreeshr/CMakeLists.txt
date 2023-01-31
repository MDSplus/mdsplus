
###
### XTreeShr
###

file(GLOB
    _source_list
    "*.c"
)

add_library(
    XTreeShr
    ${_source_list}
)

install(TARGETS XTreeShr)

target_link_libraries(
    XTreeShr
    PUBLIC
        TdiShr
)

add_static_copy(XTreeShr)