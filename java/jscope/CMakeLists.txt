
###
### jScope.jar
###

# MANIFEST.MF

set(_manifest ${CMAKE_CURRENT_BINARY_DIR}/MANIFEST.MF)

file(WRITE ${_manifest}
    "Specification-Version: ${RELEASE_VERSION}\n"
    "Implementation-Version: ${RELEASE_VERSION}\n"
    "Implementation-Vendor-Id: org.mdsplus\n"
    "Class-Path: jsch-0.1.54.jar mdsobjects.jar\n"
)

file(GLOB_RECURSE
    _source_list
    "src/main/java/debug/*.java"
    "src/main/java/mds/connection/*.java"
    "src/main/java/mds/jscope/*.java"
    "src/main/java/mds/provider/*.java"
    "src/main/java/mds/wave/*.java"
)

file(GLOB_RECURSE
    _jscope_resource_list
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/main/resources/mds/jscope/*"
)

file(GLOB_RECURSE
    _wave_resource_list_jscope
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/main/resources/mds/wave/*"
)

add_jar(
    jScope
    SOURCES ${_source_list}
    RESOURCES
        NAMESPACE "mds/jscope" ${_jscope_resource_list}
        NAMESPACE "mds/wave" ${_wave_resource_list_jscope}
    INCLUDE_JARS ${JSCH} mdsobjects
    ENTRY_POINT mds.jscope.jScope
    MANIFEST ${_manifest}
)

install_jar(
    jScope
    DESTINATION java/classes
)

install(
    FILES src/main/resources/mds/jscope/jScope.properties
    DESTINATION java/classes
)

# Because of the code in DataServerItem.java, we need to ensure that the zip (jar) is generated
# with explicit entries for directories, instead of just implicit ones. When calling `jar -C`,
# this happens automatically, but CMake calls `jar @java_class_files` which creates a jar without
# the directory entries. To get around this, we simply unzip and rezip the jar file ourselves.
# Unfortunately, this causes DeviceBeans.jar to be rebuilt every time you build.

get_property(_jar_file TARGET jScope PROPERTY JAR_FILE)

set(_tmp "${CMAKE_CURRENT_BINARY_DIR}/tmp")
file(MAKE_DIRECTORY "${_tmp}")

add_custom_command(
    TARGET jScope POST_BUILD
    COMMAND ${Java_JAR_EXECUTABLE} xf ${_jar_file}
    COMMAND ${Java_JAR_EXECUTABLE} cemf mds.jscope.jScope ${_manifest} ${_jar_file} -C ${_tmp} .
    WORKING_DIRECTORY ${_tmp}
)

###
### Scripts
###

if(WIN32)
    set(_script_list
        jScope.bat
    )
else()
    set(_script_list
        jScope
    )
endif()

file(COPY ${_script_list}
    DESTINATION ${CMAKE_BINARY_DIR}
)

install(
    FILES ${_script_list}
    TYPE BIN
    PERMISSIONS ${MODE_755}
)