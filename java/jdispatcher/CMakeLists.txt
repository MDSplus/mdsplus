
###
### jDispatcher.jar
###

# MANIFEST.MF

set(_manifest ${CMAKE_CURRENT_BINARY_DIR}/MANIFEST.MF)

file(WRITE ${_manifest}
    "Specification-Version: ${RELEASE_VERSION}\n"
    "Implementation-Version: ${RELEASE_VERSION}\n"
    "Implementation-Vendor-Id: org.mdsplus\n"
    "Class-Path: mdsobjects.jar\n"
)

file(GLOB_RECURSE
    _source_list
    "src/main/java/mds/jdispatcher/*.java"
)


add_jar(
    jDispatcher
    SOURCES ${_source_list}
    INCLUDE_JARS mdsobjects
    ENTRY_POINT mds.jscope.jDispatcher
    MANIFEST ${_manifest}
)

install_jar(
    jDispatcher
    DESTINATION java/classes
)

install(
    FILES src/main/resources/mds/jdispatcher/jDispatcher.properties
    DESTINATION java/classes
)

###
### Scripts
###

if(WIN32)
    set(_script_list
        jDispatcherIp.bat
        jDispatchMonitor.bat
        jServer.bat
    )
else()
    set(_script_list
        jDispatcherIp
        jDispatchMonitor
        jServer
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