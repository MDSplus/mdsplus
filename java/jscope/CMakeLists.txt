
###
### jscope.jar
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
    "src/main/java/*"
)

file(GLOB_RECURSE
    _jscope_resource_list
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/main/resources/mds/jscope/*"
)

file(GLOB_RECURSE
    _wave_resource_list_jscope
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/main/resources/mds/wave/*"
)

add_jar(
    jScope
    SOURCES ${_source_list}
    RESOURCES
        NAMESPACE "mds/jscope" ${_jscope_resource_list}
        NAMESPACE "mds/wave" ${_wave_resource_list_jscope}
    INCLUDE_JARS ${JSCH} mdsobjects
    ENTRY_POINT mds.jscope.jScope
    MANIFEST ${_manifest}
)

install_jar(
    jScope
    DESTINATION java/classes
)

fat_jar(jScope ${JSCH})

###
### Scripts
###

if(WIN32)
    install(
        FILES jScope.bat
        TYPE BIN
    )
else()
    install(
        FILES jScope.template RENAME jScope
        TYPE BIN
        PERMISSIONS ${MODE_755}
    )
endif()