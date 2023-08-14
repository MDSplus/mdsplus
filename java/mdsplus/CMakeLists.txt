
###
### MdsPlus.jar
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
    MdsPlus
    SOURCES ${_source_list}
    MANIFEST ${_manifest}
)

install_jar(
    MdsPlus
    DESTINATION java/classes
)