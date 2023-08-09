
set(_test_source_list
    dtype_test.c
    mdslib_ctest.c
    # mdslib_ftest.f
    # mdslib_fremotetest.f
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
            MdsLib
            MdsTestShr
    )

    # TODO: Remove?
    target_compile_options(
        ${_test_name}
        PRIVATE
            -Wno-error
    )

    mdsplus_add_test(
        NAME ${_test_name}
        COMMAND ${_test_name}
    )

endforeach()
