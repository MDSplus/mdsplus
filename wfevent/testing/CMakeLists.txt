
file(GLOB
    _test_script_list
    "*.sh"
)

foreach(_test_script IN LISTS _test_script_list)

    cmake_path(GET _test_script STEM _target)

    mdsplus_add_test(
        NAME "${_target}"
        COMMAND ${_test_script}
        NO_VALGRIND
        NO_WINE
    )

endforeach()
