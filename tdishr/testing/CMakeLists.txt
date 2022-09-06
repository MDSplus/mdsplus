
###
### tdishr_test
###

ADD_EXECUTABLE(
    build_test
    build_test.c
)

# (SLW) TODO: Should we install this?
INSTALL(TARGETS build_test)

TARGET_LINK_LIBRARIES(
    build_test
    PRIVATE
        TdiShr
        MdsTestShr
)

ADD_TEST(
    NAME build_test
    COMMAND $<TARGET_FILE:build_test>
)