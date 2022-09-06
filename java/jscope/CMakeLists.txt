
###
### jscope.jar
###

# MANIFEST.MF

SET(_manifest ${CMAKE_CURRENT_BINARY_DIR}/MANIFEST.MF)

FILE(WRITE ${_manifest}
    "Specification-Version: ${RELEASE_VERSION}\n"
    "Implementation-Version: ${RELEASE_VERSION}\n"
    "Implementation-Vendor-Id: org.mdsplus\n"
    "Class-Path: mdsobjects.jar\n"
)

FILE(GLOB_RECURSE
    _source_list
    "src/main/java/*"
)

FILE(GLOB_RECURSE
    _jscope_resource_list
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/main/resources/mds/jscope/*"
)

FILE(GLOB_RECURSE
    _wave_resource_list_jscope
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/main/resources/mds/wave/*"
)

ADD_JAR(
    jScope
    SOURCES ${_source_list}
    RESOURCES
        NAMESPACE "mds/jscope" ${_jscope_resource_list}
        NAMESPACE "mds/wave" ${_wave_resource_list_jscope}
    INCLUDE_JARS ${JSCH} mdsobjects
    ENTRY_POINT mds.jscope.jScope
    MANIFEST ${_manifest}
)

INSTALL_JAR(
    jScope
    DESTINATION java/classes
)

FAT_JAR(jScope ${JSCH})

###
### Scripts
###

IF(WIN32)
    INSTALL(
        FILES jScope.bat
        TYPE BIN
    )
ELSE()
    INSTALL(
        FILES jScope.template RENAME jScope
        TYPE BIN
    )
ENDIF()