
###
### tditest
###

add_executable(
    tditest
    tditest.c
)

install(TARGETS tditest)

target_link_libraries(
    tditest
    PUBLIC
        TdiShr
        Readline::Readline
)
