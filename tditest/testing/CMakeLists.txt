
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

    if(_target STREQUAL "test-dev-py")
        # Not supported on windows
        if(WIN32)
            continue()
        endif()

        # Hangs with the thread sanitizer in `Tcl('add node a12/model=a12')`
        if(ENABLE_SANITIZE STREQUAL "thread")
            continue()
        endif()
    endif()

    mdsplus_add_test(
        NAME "${_target}"
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/do_tditests.sh ${_test_script}
        NO_VALGRIND # TODO: Turn this back on after upgrading do_tditests.sh
        NO_WINE
    )

endforeach()
