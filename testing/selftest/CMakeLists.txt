
add_library(
    MdsTestDummy SHARED
    mdstestdummy.h
    mdstestdummy.cpp
)

set(_test_source_list
    test_options.c
    example2.c
    example3.cpp
    example4.c
    example_assert.c
    # TODO: Support TAP error codes?
    # skip_test.c
    # error_test.c # segfault?
    fork_success.c
    fork_fail.c
    timeout_fail.c
)

set(_python_test_source_list
    pyex1.py
    py_fail.py
)

set(_should_fail_source_list
    example4.c
    example_assert.c
    fork_fail.c
    timeout_fail.c
    py_fail.py

    # TODO: Support TAP error codes?
    skip_test.c
    error_test.c
)

if(ENABLE_VALGRIND)

    list(APPEND _test_source_list
        leak_test.c
    )

    list(APPEND _should_fail_source_list
        leak_test.c
    )

else()

    list(APPEND _test_source_list
        timeout_success.c
    )

endif()


foreach(_test_source ${_test_source_list})

    cmake_path(GET _test_source STEM _test_name)

    add_executable(
        ${_test_name}
        ${_test_source}
    )

    target_link_libraries(
        ${_test_name}
        PRIVATE
            MdsTestShr
    )

    mdsplus_add_test(
        NAME ${_test_name}
        COMMAND $<TARGET_FILE:${_test_name}>
        TEST_LIST_VARIABLE _test_target_list
    )

    if(_test_source IN_LIST _should_fail_source_list)
        list(APPEND _should_fail_target_list ${_test_target_list})
    endif()

endforeach()

target_link_libraries(
    example_assert
    PRIVATE
        MdsTestDummy
)

foreach(_python_test_source ${_python_test_source_list})

    cmake_path(GET _python_test_source STEM _test_name)

    mdsplus_add_test(
        NAME ${_test_name}
        COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${_python_test_source}
        TEST_LIST_VARIABLE _test_target_list
    )

    if(_python_test_source IN_LIST _should_fail_source_list)
        list(APPEND _should_fail_target_list ${_test_target_list})
    endif()

endforeach()

set_tests_properties(
    ${_should_fail_target_list}
    PROPERTIES
        WILL_FAIL TRUE
)