
###
### jTraverser2.jar
###

# MANIFEST.MF

SET(_manifest ${CMAKE_CURRENT_BINARY_DIR}/MANIFEST.MF)

FILE(WRITE ${_manifest}
    "Specification-Version: ${RELEASE_VERSION}\n"
    "Implementation-Version: ${RELEASE_VERSION}\n"
    "Implementation-Vendor-Id: org.mdsplus\n"
    "Class-Path: mdsplus-api.jar DeviceBeans.jar WaveDisplay.jar mdsobjects.jar jDevices.jar\n"
)

FILE(GLOB_RECURSE
    _source_list
    "src/main/java/*"
)

FILE(GLOB_RECURSE
    _jtraverser_resource_list
    "src/main/resources/mds/jtraverser/*"
)

ADD_JAR(
    jTraverser2
    SOURCES ${_source_list}
    RESOURCES
        NAMESPACE "mds/jtraverser" ${_jtraverser_resource_list}
    INCLUDE_JARS mdsplus-api
    ENTRY_POINT mds.jtraverser.jTraverser
    MANIFEST ${_manifest}
)

INSTALL_JAR(
    jTraverser2
    DESTINATION java/classes
)

###
### Scripts
###

IF(WIN32)
    INSTALL(
        FILES
            jTraverser2.bat
            setupDevice.bat
        TYPE BIN
    )
ELSE()
    INSTALL(
        FILES jTraverser2.template RENAME jTraverser2
        TYPE BIN
    )
    INSTALL(
        FILES setupDevice.template RENAME setupDevice
        TYPE BIN
    )
ENDIF()