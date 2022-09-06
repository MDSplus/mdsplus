
###
### mdsobjects.jar
###

SET(JAVAX_JSON "${CMAKE_CURRENT_SOURCE_DIR}/src/main/resources/javax.json-1.0.4.jar")

# MANIFEST.MF

SET(_manifest ${CMAKE_CURRENT_BINARY_DIR}/MANIFEST.MF)

FILE(WRITE ${_manifest}
    "Specification-Version: ${RELEASE_VERSION}\n"
    "Implementation-Version: ${RELEASE_VERSION}\n"
    "Implementation-Vendor-Id: org.mdsplus\n"
)

FILE(GLOB_RECURSE
    _source_list
    "src/main/java/*"
)

FILE(GLOB_RECURSE
    _resource_list
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/src/main/resources/
    "src/main/resources/*"
)

ADD_JAR(
    mdsobjects
    SOURCES ${_source_list} ${_resource_list}
    INCLUDE_JARS ${JAVAX_JSON}
    MANIFEST ${JAVA_MANIFEST}
)

INSTALL_JAR(
    mdsobjects
    DESTINATION java/classes
)

FAT_JAR(mdsobjects ${JAVAX_JSON})