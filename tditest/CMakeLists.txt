
###
### tditest
###

add_executable(
    tditest
    tditest.c
)

target_link_libraries(
    tditest
    PUBLIC
        TdiShr
        Readline::Readline
)

install(TARGETS tditest)
