
###
### mdsobjects.jar
###

set(JAVAX_JSON "${CMAKE_CURRENT_SOURCE_DIR}/src/main/resources/javax.json-1.0.4.jar")

# MANIFEST.MF

set(_manifest ${CMAKE_CURRENT_BINARY_DIR}/MANIFEST.MF)

file(WRITE ${_manifest}
    "Specification-Version: ${RELEASE_VERSION}\n"
    "Implementation-Version: ${RELEASE_VERSION}\n"
    "Implementation-Vendor-Id: org.mdsplus\n"
)

file(GLOB_RECURSE
    _source_list
    "src/main/java/*"
)

file(GLOB_RECURSE
    _resource_list
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/src/main/resources/
    "src/main/resources/*"
)

add_jar(
    mdsobjects
    SOURCES ${_source_list} ${_resource_list}
    INCLUDE_JARS ${JAVAX_JSON}
    MANIFEST ${JAVA_MANIFEST}
)

install_jar(
    mdsobjects
    DESTINATION java/classes
)

fat_jar(mdsobjects ${JAVAX_JSON})

###
### Testing
###

file(GLOB_RECURSE
    _source_list
    "src/test/java/*.java"
)

file(GLOB_RECURSE
    _test_jar_list
    "../tests/*.jar"
)

add_jar(
    mdsobjects-tests
    SOURCES ${_source_list}
    INCLUDE_JARS ${_test_jar_list} mdsobjects taptest
)

foreach(_test_jar IN LISTS _test_jar_list)
    fat_jar(mdsobjects-tests ${_test_jar})
endforeach()

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

foreach(_test_class IN LISTS _test_class_list)

    add_test(
        NAME "${_test_class}"
        # COMMAND ${Java_JAVA_EXECUTABLE}
        COMMAND ${CMAKE_SOURCE_DIR}/java/junittest ${_test_class}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    list(APPEND _test_target_list ${_test_class})

endforeach()

include(SetEnvironmentForTests)

set(_env_mods
    "java_test_path=set:${CMAKE_CURRENT_BINARY_DIR}"
    "CLASSPATH=path_list_append:${CMAKE_CURRENT_BINARY_DIR}/mdsobjects.jar"
    "CLASSPATH=path_list_append:${CMAKE_CURRENT_BINARY_DIR}/mdsobjects-tests.jar"
)

# hack
foreach(_test_jar IN LISTS _test_jar_list)
    list(APPEND _env_mods
        "CLASSPATH=path_list_append:${_test_jar}"
    )
endforeach()

set_environment_for_tests(${_test_target_list} "${_extra_env_mods}")