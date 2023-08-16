
if(ENABLE_JAVA)

    file(GLOB_RECURSE
        _source_list
        "../src/test/*.java"
    )

    add_jar(
        mdsobjects-tests
        SOURCES ${_source_list}
        INCLUDE_JARS mdsobjects ${JAVA_TEST_JAR_LIST}
    )

    set(_test_class_list
        MDSplus.MdsDataTest
        MDSplus.MdsConglomTest
        MDSplus.MdsConnectionTest
        MDSplus.MdsDimensionTest
        MDSplus.MdsEventTest
        MDSplus.MdsExpressionCompileTest
        MDSplus.MdsFunctionTest
        MDSplus.MdsRangeTest
        MDSplus.MdsSignalTest
        MDSplus.MdsStringTest
        MDSplus.MdsTreeNodeTest
        MDSplus.MdsTreeTest
        MDSplus.MdsWindowTest
    )

    list(APPEND JAVA_TEST_ENV_MODS
        "java_test_path=set:${CMAKE_CURRENT_BINARY_DIR}"
        "CLASSPATH=path_list_append:${CMAKE_CURRENT_BINARY_DIR}/../mdsobjects.jar"
        "CLASSPATH=path_list_append:${CMAKE_CURRENT_BINARY_DIR}/mdsobjects-tests.jar"
    )

    foreach(_test_class IN LISTS _test_class_list)

        mdsplus_add_test(
            NAME ${_test_class}
            # TODO: Call ${Java_JAVA_EXECUTABLE} directly
            COMMAND ${CMAKE_SOURCE_DIR}/java/junittest ${_test_class}
            ENVIRONMENT_MODIFICATION ${JAVA_TEST_ENV_MODS}
        )

    endforeach()

endif()