
###
### mdsobjects.jar
###

# MANIFEST.MF

set(_manifest ${CMAKE_CURRENT_BINARY_DIR}/MANIFEST.MF)

file(WRITE ${_manifest}
    "Specification-Version: ${RELEASE_VERSION}\n"
    "Implementation-Version: ${RELEASE_VERSION}\n"
    "Implementation-Vendor-Id: org.mdsplus\n"
    "Class-Path: javax.json-1.0.4.jar\n"
)

file(GLOB_RECURSE
    _source_list
    "src/main/java/*.java"
)

add_jar(
    mdsobjects
    SOURCES ${_source_list}
    INCLUDE_JARS ${JAVAX_JSON}
    MANIFEST ${_manifest}
)

install_jar(
    mdsobjects
    DESTINATION java/classes
)
