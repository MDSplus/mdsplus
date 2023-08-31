
###
### wfevent
###

add_executable(
    wfevent
    wfevent.c
)

target_link_libraries(
    wfevent
    PUBLIC
        TdiShr
)

install(TARGETS wfevent)
