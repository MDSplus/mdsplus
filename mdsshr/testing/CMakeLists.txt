
set(_test_source_list
    UdpEventsTest.c
    UdpEventsTestStatics.c
)

list(APPEND Valgrind_SUPPRESSION_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/valgrind.supp
)

foreach(_test_source ${_test_source_list})

    cmake_path(GET _test_source STEM _test_name)

    add_executable(
        ${_test_name}
        ${_test_source}
    )

    target_link_libraries(
        ${_test_name}
        PRIVATE
            MdsShr
            MdsTestShr
    )

    mdsplus_add_test(
        NAME "${_test_name}"
        COMMAND $<TARGET_FILE:${_test_name}>
    )

endforeach()

# # We need to run these serially so the don't use the same socket
# set_tests_properties(
#     mdsshr/testing/UdpEventsTest
#     mdsshr/testing/UdpEventsTestStatics
#     PROPERTIES 
#         RUN_SERIAL TRUE
# )
