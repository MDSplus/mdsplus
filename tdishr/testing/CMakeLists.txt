
set(_test_source_list
    build_test.c
)

foreach(_test_source ${_test_source_list})

    cmake_path(GET _test_source STEM _test_name)

    add_executable(
        ${_test_name}
        ${_test_source}
    )

    target_link_libraries(
        ${_test_name}
        PRIVATE
            TdiShr
            MdsTestShr
            LibM::LibM
    )

    add_test(
        NAME ${_test_name}
        COMMAND $<TARGET_FILE:${_test_name}>
    )

    list(APPEND _test_target_list ${_test_name})

endforeach()

include(SetEnvironmentForTests)

set_environment_for_tests(${_test_target_list})

# (SLW) TODO: Should we install this?
install(TARGETS build_test)
