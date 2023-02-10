
SET(_env_mods
    "PYTHONPATH=set:${CMAKE_SOURCE_DIR}/python"
)

IF(WIN32)
    LIST(APPEND _env_mods "PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
ELSE()
    LIST(APPEND _env_mods "LD_LIBRARY_PATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
ENDIF()

SET(_test_case_list
    "data_case.py"
    "devices_case.py"
    "exception_case.py"
    "segment_case.py"
    "task_case.py"
    "tree_case.py"
    "connection_case.py"
    "dcl_case.py"
)

FOREACH(_test_case IN LISTS _test_case_list)
    EXECUTE_PROCESS(
        COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/python/get_tests_for_case.py ${_test_case}
        OUTPUT_VARIABLE _test_list
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    CMAKE_PATH(GET _test_case STEM _case)
    STRING(REPLACE "_case" "" _case "${_case}")

    FOREACH(_test IN LISTS _test_list)
        SET(_target "py-${_case}-${_test}")

        ADD_TEST(
            NAME "${_target}"
            COMMAND ${Python_EXECUTABLE} ${_test_case} ${_test}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        
        LIST(APPEND _test_target_list ${_target})
    ENDFOREACH()
ENDFOREACH()

IF(WIN32)
    LIST(REMOVE_ITEM _test_target_list "python-dcl-dispatcher")
ENDIF()

SET_TESTS_PROPERTIES(
    ${_test_target_list}
    PROPERTIES
        ENVIRONMENT_MODIFICATION "${_env_mods}"
)