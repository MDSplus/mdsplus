
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

target_link_libraries(
    wfevent
    PUBLIC
        TdiShr
)

install(TARGETS wfevent)
