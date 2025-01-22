
file(GLOB
    _test_script_list
    "test-*.tdi"
)

# Skipping child programs that are launched inside python classes
# list(APPEND Valgrind_FLAGS
#     --trace-children-skip=*/diff
# )

foreach(_test_script IN LISTS _test_script_list)

    cmake_path(GET _test_script STEM _target)

    # Not supported on windows
    if(WIN32 AND _target STREQUAL "test-dev-py")
        continue()
    endif()

    mdsplus_add_test(
        NAME "${_target}"
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/do_tditests.sh ${_test_script}
        NO_VALGRIND # (SLW) TODO: Turn this back on after upgrading do_tditests.sh
        NO_WINE
    )

endforeach()
