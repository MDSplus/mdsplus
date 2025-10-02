
###
### mdsobjects.jar
###

# TODO: Move to 3rd-party-apis?
set(_javax_json_jar "${CMAKE_CURRENT_SOURCE_DIR}/src/main/resources/javax.json-1.0.4.jar")

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
    INCLUDE_JARS ${_javax_json_jar}
    MANIFEST ${_manifest}
)

install_jar(
    mdsobjects
    DESTINATION java/classes
)
