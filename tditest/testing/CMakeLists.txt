
file(GLOB
    _test_script_list
    "test-*.tdi"
)

foreach(_test_script IN LISTS _test_script_list)

    cmake_path(GET _test_script STEM _target)

    mdsplus_add_test(
        NAME "${_target}"
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/do_tditests.sh ${_test_script}
    )

endforeach()
