
set(_test_case_list
    "data_case.py"
    "devices_case.py"
    "exception_case.py"
    "segment_case.py"
    "task_case.py"
    "tree_case.py"
    "connection_case.py"
    "dcl_case.py"
)

foreach(_test_case IN LISTS _test_case_list)
    execute_process(
        COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/python/get_tests_for_case.py ${_test_case}
        OUTPUT_VARIABLE _test_list
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    cmake_path(GET _test_case STEM _case)
    string(REPLACE "_case" "" _case "${_case}")

    foreach(_test IN LISTS _test_list)
        set(_target "${_case}-${_test}")

        if(WIN32 AND _target STREQUAL "dcl-dispatcher")
            continue()
        endif()

        mdsplus_add_test(
            NAME "${_target}"
            COMMAND ${Python_EXECUTABLE} ${_test_case} ${_test}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endforeach()
endforeach()