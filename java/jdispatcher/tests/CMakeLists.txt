
if(ENABLE_JAVA AND NOT TEST_WITH_WINE AND NOT ENABLE_SANITIZE)

    file(GLOB_RECURSE
        _source_list
        "../src/test/*.java"
    )

    add_jar(
        jdispatcher-tests
        SOURCES ${_source_list}
        INCLUDE_JARS jDispatcher mdsobjects ${JAVA_TEST_JAR_LIST}
    )

    set(_test_class_list
        mds.jdispatcher.MdsServerMockTest
    )

    list(APPEND JAVA_TEST_ENV_MODS
        "CLASSPATH=path_list_prepend:${CMAKE_BINARY_DIR}/java/mdsobjects/mdsobjects.jar"
        "CLASSPATH=path_list_prepend:${CMAKE_CURRENT_BINARY_DIR}/../jDispatcher.jar"
        "CLASSPATH=path_list_prepend:${CMAKE_CURRENT_BINARY_DIR}/jdispatcher-tests.jar"
    )

    foreach(_test_class IN LISTS _test_class_list)

        mdsplus_add_test(
            NAME ${_test_class}
            COMMAND ${Java_JAVA_EXECUTABLE} ${JAVA_TEST_ARGUMENTS} org.junit.runner.JUnitCore ${_test_class}
            ENVIRONMENT_MODIFICATION ${JAVA_TEST_ENV_MODS}
        )

    endforeach()

endif()
