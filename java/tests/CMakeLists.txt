
if(ENABLE_JAVA AND NOT TEST_WITH_WINE AND NOT ENABLE_SANITIZE)

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

    set(JAVA_TEST_ARGUMENTS
        # Java doesn't respect [DY]LD_LIBRARY_PATH, so we need to set it directly
        -Djava.library.path=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    )

    if(ENABLE_VALGRIND)

        # Disable Java runtime optimizations / Just-In-Time (JIT) compilation
        # as they interfere with valgrind
        list(APPEND JAVA_TEST_ARGUMENTS
            -Xint
            -XX:UseSSE=0
            -Djava.compiler=NONE
        )

        if(Java_VERSION VERSION_GREATER_EQUAL 13)
            # Starting with Java 13, Java defaults to posix_spawn instead of vfork,
            # which runs a tool called jspawnhelper and cannot run with valgrind
            list(APPEND JAVA_TEST_ARGUMENTS
                -Djdk.lang.Process.launchMechanism=vfork
            )
        endif()

    endif()

    # HACK
    set(JAVA_TEST_JAR_LIST taptest ${_test_jar_list} PARENT_SCOPE)
    set(JAVA_TEST_ENV_MODS "${JAVA_TEST_ENV_MODS}" PARENT_SCOPE)
    set(JAVA_TEST_ARGUMENTS "${JAVA_TEST_ARGUMENTS}" PARENT_SCOPE)

endif()