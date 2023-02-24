
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