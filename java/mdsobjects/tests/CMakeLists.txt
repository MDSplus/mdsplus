
if(ENABLE_JAVA AND NOT TEST_WITH_WINE AND NOT ENABLE_SANITIZE)

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

    list(APPEND Valgrind_SUPPRESSION_FILES
        ${CMAKE_SOURCE_DIR}/java/valgrind.supp
    )

    list(APPEND JAVA_TEST_ENV_MODS
        "CLASSPATH=path_list_append:${CMAKE_CURRENT_BINARY_DIR}/../mdsobjects.jar"
        "CLASSPATH=path_list_append:${CMAKE_CURRENT_BINARY_DIR}/mdsobjects-tests.jar"
    )

    foreach(_test_class IN LISTS _test_class_list)

        if(ENABLE_SANITIZE STREQUAL "thread" AND 
            _test_class STREQUAL "MDSplus.MdsConnectionTest")
            continue()
        endif()

        mdsplus_add_test(
            NAME ${_test_class}
            COMMAND ${Java_JAVA_EXECUTABLE} ${JAVA_TEST_ARGUMENTS} org.junit.runner.JUnitCore ${_test_class}
            ENVIRONMENT_MODIFICATION ${JAVA_TEST_ENV_MODS}
        )

    endforeach()

endif()