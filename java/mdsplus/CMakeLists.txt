
###
### MdsPlus.jar
###

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

ADD_JAR(
    MdsPlus
    SOURCES ${_source_list}
    MANIFEST ${_manifest}
)

INSTALL_JAR(
    MdsPlus
    DESTINATION java/classes
)