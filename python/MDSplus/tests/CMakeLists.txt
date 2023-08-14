
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
        set(_target "py-${_case}-${_test}")
        # set(_target "python3 ${_test_case} ${_test}")

        add_test(
            NAME "${_target}"
            COMMAND ${Python_EXECUTABLE} ${_test_case} ${_test}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        
        list(APPEND _test_target_list ${_target})
    endforeach()
endforeach()

if(WIN32)
    list(REMOVE_ITEM _test_target_list "python-dcl-dispatcher")
endif()

# TODO: Remove when we can finally fix this
list(REMOVE_ITEM _test_target_list "py-task-do")

include(SetEnvironmentForTests)

set_environment_for_tests(${_test_target_list})
