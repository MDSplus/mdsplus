
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

target_link_libraries(
    XTreeShr
    PUBLIC
        TdiShr
)

target_compile_definitions(
    XTreeShr
    PRIVATE
        -DLIBPREFIX=XTree
)

mdsplus_add_static_copy(XTreeShr)

install(TARGETS XTreeShr XTreeShr-static)