
###
### wfevent
###

file(GLOB
    _source_list
    "*.c"
)

add_executable(
    wfevent
    ${_source_list}
)

install(TARGETS wfevent)

target_link_libraries(
    wfevent
    PUBLIC
        TdiShr
)
