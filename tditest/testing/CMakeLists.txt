
file(GLOB
    _test_script_list
    "test-*.tdi"
)

# if(NOT TARGET MitDevices)
#     list(REMOVE_ITEM "test-dev-py.tdi")
# endif()

# Skipping child programs that are launched inside python classes
# list(APPEND Valgrind_FLAGS
#     --trace-children-skip=*/diff
# )

foreach(_test_script IN LISTS _test_script_list)

    cmake_path(GET _test_script STEM _target)

    mdsplus_add_test(
        NAME "${_target}"
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/do_tditests.sh ${_test_script}
        NO_VALGRIND # (SLW) TODO: Turn this back on after upgrading do_tditests.sh
    )

endforeach()
