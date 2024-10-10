
###
### DeviceBeans.jar
###

file(GLOB_RECURSE
    _source_list
    "src/main/java/*.java"
)

file(GLOB_RECURSE
    _resource_list
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/main/resources/*"
)

add_jar(
    jDevices
    SOURCES ${_source_list}
    RESOURCES
        NAMESPACE "" ${_resource_list}
    INCLUDE_JARS DeviceBeans
)

install_jar(
    jDevices
    DESTINATION java/classes
)