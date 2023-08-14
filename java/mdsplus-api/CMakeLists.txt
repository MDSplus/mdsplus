
###
### mdsplus-api.jar
###

# MANIFEST.MF

set(_manifest ${CMAKE_CURRENT_BINARY_DIR}/MANIFEST.MF)

file(WRITE ${_manifest}
    "Specification-Version: ${RELEASE_VERSION}\n"
    "Implementation-Version: ${RELEASE_VERSION}\n"
    "Implementation-Vendor-Id: org.mdsplus\n"
)

file(GLOB_RECURSE
    _source_list
    "src/main/java/*.java"
)

add_jar(
    mdsplus-api
    SOURCES ${_source_list} # ${_jsch_source_list}
    INCLUDE_JARS ${JSCH}
    MANIFEST ${_manifest}
)

install_jar(
    mdsplus-api
    DESTINATION java/classes
)

fat_jar(mdsplus-api ${JSCH})

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
    mdsplus-api-tests
    SOURCES ${_source_list}
    INCLUDE_JARS ${_test_jar_list} mdsplus-api taptest
)

foreach(_test_jar IN LISTS _test_jar_list)
    fat_jar(mdsplus-api-tests ${_test_jar})
endforeach()

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
    "CLASSPATH=path_list_append:${CMAKE_CURRENT_BINARY_DIR}/mdsplus-api.jar"
    "CLASSPATH=path_list_append:${CMAKE_CURRENT_BINARY_DIR}/mdsplus-api-tests.jar"
)

# hack
foreach(_test_jar IN LISTS _test_jar_list)
    list(APPEND _env_mods
        "CLASSPATH=path_list_append:${_test_jar}"
    )
endforeach()

set_environment_for_tests(${_test_target_list} "${_extra_env_mods}")