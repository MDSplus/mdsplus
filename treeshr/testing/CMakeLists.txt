
set(_test_source_list
    TreeDeleteNodeTest.c
    TreeSegmentTest.c
    TreeFileIoTest.c
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
            TreeShr
            MdsTestShr
    )

    set(_no_valgrind)
    if(_test_source STREQUAL "TreeSegmentTest.c")
        set(_no_valgrind NO_VALGRIND)
    endif()

    mdsplus_add_test(
        NAME ${_test_name}
        COMMAND $<TARGET_FILE:${_test_name}>
        ${_no_valgrind}
    )

endforeach()

###
### Reference TreeFileIo backend for TreeFileIoTest (scheme "testmem://")
###
# A loadable backend the seam test dlopens by image name "MdsTreeFileTESTMEM"
# (-> libMdsTreeFileTESTMEM.so), exercising GetTreeFileIo() dispatch through a
# non-default backend. It builds into CMAKE_LIBRARY_OUTPUT_DIRECTORY, which
# mdsplus_add_test() already puts on MDSPLUS_LIBRARY_PATH / LD_LIBRARY_PATH, so
# the loader finds it at test time with no extra environment wiring.
add_library(
    MdsTreeFileTESTMEM
    MdsTreeFileTESTMEM.c
)

target_include_directories(
    MdsTreeFileTESTMEM
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Ensure the backend is built before the test that loads it runs.
add_dependencies(TreeFileIoTest MdsTreeFileTESTMEM)