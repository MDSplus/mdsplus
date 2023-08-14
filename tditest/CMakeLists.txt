
###
### tditest
###

add_executable(
    tditest
    tditest.c
)

target_link_libraries(
    tditest
    PUBLIC
        TdiShr
        Readline::Readline
)

install(TARGETS tditest)

###
### Testing
###

file(GLOB
    _test_script_list
    "testing/test-*.tdi"
)

foreach(_test_script IN LISTS _test_script_list)

    cmake_path(GET _test_script STEM _target)

    add_test(
        NAME "${_target}"
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testing/do_tditests.sh ${_test_script}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    list(APPEND _test_target_list ${_target})
endforeach()

include(SetEnvironmentForTests)

set_environment_for_tests(${_test_target_list})
