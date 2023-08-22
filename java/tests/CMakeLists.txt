
if(ENABLE_JAVA)

    ###
    ### taptest.jar
    ###

    list(APPEND CMAKE_JAVA_COMPILE_FLAGS -source 8)

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
        # Used to run Java, java.library.path is for loading shared libraries
        "JAVA_PATH_NAME=set:${Java_JAVA_EXECUTABLE} -Djava.library.path=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"

        # Used to call our custom TapTestRunner instead of the JUnit default
        "JUNITTEST=set:org.junit.runner.TapTestRunner"
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