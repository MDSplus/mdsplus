
if(ENABLE_JAVA)

    ###
    ### taptest.jar
    ###

    file(GLOB
        _test_jar_list
        "*.jar"
    )

    add_jar(
        taptest
        SOURCES
            org/junit/runner/TapTestRunner.java
        INCLUDE_JARS ${_test_jar_list}
    )

    set(JAVA_TEST_ENV_MODS
        "CLASSPATH=path_list_append:${CMAKE_CURRENT_BINARY_DIR}/taptest.jar"
    )

    foreach(_test_jar IN LISTS _test_jar_list)
        list(APPEND JAVA_TEST_ENV_MODS
            "CLASSPATH=path_list_append:${_test_jar}"
        )
    endforeach()

    # HACK
    set(JAVA_TEST_JAR_LIST taptest ${_test_jar_list} PARENT_SCOPE)
    set(JAVA_TEST_ENV_MODS "${JAVA_TEST_ENV_MODS}" PARENT_SCOPE)

endif()