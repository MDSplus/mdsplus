
set(_test_source_list
    TreeDeleteNodeTest.c
    TreeSegmentTest.c
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
            TreeShr
            MdsTestShr
    )

    set(_no_valgrind)
    if(_test_source STREQUAL "TreeSegmentTest.c")
        set(_no_valgrind NO_VALGRIND)
    endif()

    mdsplus_add_test(
        NAME ${_test_name}
        COMMAND $<TARGET_FILE:${_test_name}>
        ${_no_valgrind}
    )

endforeach()