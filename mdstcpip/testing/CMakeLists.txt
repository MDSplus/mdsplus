
# TODO: mdscp ?

###
### MdsIpTest
###

add_executable(
    MdsIpTest
    MdsIpTest.c
)

target_link_libraries(
    MdsIpTest
    PRIVATE
        MdsIpShr
        MdsTestShr
)

add_test(
    NAME MdsIpTest
    COMMAND $<TARGET_FILE:MdsIpTest>
)

include(SetEnvironmentForTests)

set_environment_for_tests(MdsIpTest)