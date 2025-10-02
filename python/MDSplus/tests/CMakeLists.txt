
# TODO: Investigate sanitizer
if(NOT ENABLE_SANITIZE)

    set(_test_case_list
        "data_case.py"
        "devices_case.py"
        "exception_case.py"
        "segment_case.py"
        # (SLW) Currently broken by a segfault in Release builds
        "task_case.py"
        "tree_case.py"
        "connection_case.py"
        "dcl_case.py"
    )

    list(APPEND Valgrind_FLAGS
        --trace-children-skip=*/ld,*/collect2,*/ldconfig,*/sh
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

            # Currently (correctly) failing, will need to be fixed
            if(_target STREQUAL "task-do")
                continue()
            endif()

            # 32-bit tests with windows require a 32-bit python.exe
            if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4)
                continue()
            endif()
            
            set(_no_valgrind)
            if(_target STREQUAL "connection-write")
                set(_no_valgrind NO_VALGRIND)
            endif()

            mdsplus_add_test(
                NAME "${_target}"
                COMMAND ${PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/${_test_case} ${_test}
                ${_no_valgrind}
            )
        endforeach()
    endforeach()

endif()