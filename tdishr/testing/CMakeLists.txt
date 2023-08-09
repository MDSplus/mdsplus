

add_executable(
    build_test
    build_test.c
)

target_link_libraries(
    build_test
    PRIVATE
        TdiShr
        MdsTestShr
        LibM::LibM
)

mdsplus_add_test(
    NAME build_test
    COMMAND build_test
)

# (SLW) TODO: Should we install this?
install(TARGETS build_test)
