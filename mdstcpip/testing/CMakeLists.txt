
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

mdsplus_add_test(
    NAME MdsIpTest
    COMMAND MdsIpTest
)