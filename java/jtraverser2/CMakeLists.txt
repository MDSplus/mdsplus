
###
### jTraverser2.jar
###

# MANIFEST.MF

set(_manifest ${CMAKE_CURRENT_BINARY_DIR}/MANIFEST.MF)

file(WRITE ${_manifest}
    "Specification-Version: ${RELEASE_VERSION}\n"
    "Implementation-Version: ${RELEASE_VERSION}\n"
    "Implementation-Vendor-Id: org.mdsplus\n"
    "Class-Path: mdsplus-api.jar DeviceBeans.jar WaveDisplay.jar mdsobjects.jar jDevices.jar\n"
)

file(GLOB_RECURSE
    _source_list
    "src/main/java/*.java"
)

file(GLOB_RECURSE
    _jtraverser_resource_list
    "src/main/resources/mds/jtraverser/*"
)

add_jar(
    jTraverser2
    SOURCES ${_source_list}
    RESOURCES
        NAMESPACE "mds/jtraverser" ${_jtraverser_resource_list}
    INCLUDE_JARS mdsplus-api
    ENTRY_POINT mds.jtraverser.jTraverser
    MANIFEST ${_manifest}
)

install_jar(
    jTraverser2
    DESTINATION java/classes
)

###
### Scripts
###

if(WIN32)
    install(
        FILES
            jTraverser2.bat
            setupDevice.bat
        TYPE BIN
    )
else()
    install(
        FILES jTraverser2.template RENAME jTraverser2
        TYPE BIN
        PERMISSIONS ${MODE_755}
    )
    install(
        FILES setupDevice.template RENAME setupDevice
        TYPE BIN
        PERMISSIONS ${MODE_755}
    )
endif()