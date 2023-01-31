
###
### MdsMisc
###

file(GLOB
    _source_list
    "*.c"
)

add_library(
    MdsMisc
    ${_source_list}
)

install(TARGETS MdsMisc)

target_link_libraries(
    MdsMisc
    PUBLIC
        XTreeShr
)

add_static_copy(MdsMisc)
