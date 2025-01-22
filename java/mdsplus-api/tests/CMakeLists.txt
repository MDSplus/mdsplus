
if(ENABLE_JAVA AND NOT TEST_WITH_WINE AND NOT ENABLE_SANITIZE)

    file(GLOB_RECURSE
        _source_list
        "../src/test/java/*.java"
    )

    add_jar(
        mdsplus-api-tests
        SOURCES ${_source_list}
        INCLUDE_JARS mdsplus-api ${JAVA_TEST_JAR_LIST}
    )

    set(_test_class_list
        mds.Mds_Test
        mds.MdsShr_Test
        mds.TdiShr_Test
        mds.TreeShr_Test
        mds.data.CONST_Test
        mds.data.TREE_Test
        mds.data.descriptor.Descriptor_A_Test
        mds.data.descriptor.Descriptor_CA_Test
        mds.data.descriptor.Descriptor_S_Test
        mds.data.descriptor_r.Function_Test
    )

    list(APPEND Valgrind_SUPPRESSION_FILES
        ${CMAKE_SOURCE_DIR}/java/valgrind.supp
    )

    list(APPEND JAVA_TEST_ENV_MODS
        "CLASSPATH=path_list_append:${CMAKE_CURRENT_BINARY_DIR}/../mdsplus-api.jar"
        "CLASSPATH=path_list_append:${CMAKE_CURRENT_BINARY_DIR}/mdsplus-api-tests.jar"
    )

    foreach(_test_class IN LISTS _test_class_list)

        mdsplus_add_test(
            NAME ${_test_class}
            COMMAND ${Java_JAVA_EXECUTABLE} ${JAVA_TEST_ARGUMENTS} org.junit.runner.JUnitCore ${_test_class}
            ENVIRONMENT_MODIFICATION ${JAVA_TEST_ENV_MODS}
        )

    endforeach()

endif()