
###
### jTraverser.jar
###

# MANIFEST.MF

set(_manifest ${CMAKE_CURRENT_BINARY_DIR}/MANIFEST.MF)

file(WRITE ${_manifest}
    "Specification-Version: ${RELEASE_VERSION}\n"
    "Implementation-Version: ${RELEASE_VERSION}\n"
    "Implementation-Vendor-Id: org.mdsplus\n"
    "Class-Path: mdsobjects.jar DeviceBeans.jar jScope.jar jDevices.jar\n"
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
    jTraverser
    SOURCES ${_source_list}
    RESOURCES
        NAMESPACE "mds/jtraverser" ${_jtraverser_resource_list}
    INCLUDE_JARS mdsobjects
    ENTRY_POINT mds.jtraverser.jTraverser
    MANIFEST ${_manifest}
)

install_jar(
    jTraverser
    DESTINATION java/classes
)

###
### Scripts
###

if(WIN32)
    set(_script_list
        jTraverser.bat
    )
else()
    set(_script_list
        jTraverser
        CompileTree
        DecompileTree
    )
endif()

file(COPY ${_script_list}
    DESTINATION ${CMAKE_BINARY_DIR}
)

install(
    FILES ${_script_list}
    TYPE BIN
    PERMISSIONS ${MODE_755}
)