
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
