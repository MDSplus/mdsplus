
###
### mdsplus-api.jar
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
    mdsplus-api
    SOURCES ${_source_list} # ${_jsch_source_list}
    INCLUDE_JARS ${JSCH}
    MANIFEST ${_manifest}
)

INSTALL_JAR(
    mdsplus-api
    DESTINATION java/classes
)

FAT_JAR(mdsplus-api ${JSCH})