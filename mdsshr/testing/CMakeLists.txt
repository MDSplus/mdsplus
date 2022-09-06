
###
### UdpEventsTest
###

ADD_EXECUTABLE(
    UdpEventsTest
    UdpEventsTest.c
)

TARGET_LINK_LIBRARIES(
    UdpEventsTest
    PUBLIC
        MdsShr
        MdsTestShr
)

ADD_TEST(
    NAME UdpEventsTest
    COMMAND $<TARGET_FILE:UdpEventsTest>
)

###
### UdpEventsTestStatics
###

ADD_EXECUTABLE(
    UdpEventsTestStatics
    UdpEventsTestStatics.c
)

TARGET_LINK_LIBRARIES(
    UdpEventsTestStatics
    PUBLIC
        MdsShr
        MdsTestShr
)

ADD_TEST(
    NAME UdpEventsTestStatics
    COMMAND $<TARGET_FILE:UdpEventsTestStatics>
)