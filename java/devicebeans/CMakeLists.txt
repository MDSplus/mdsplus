
###
### DeviceBeans.jar
###

list(APPEND CMAKE_JAVA_COMPILE_FLAGS -source 8)

file(GLOB_RECURSE
    _source_list
    "src/main/java/*.java"
)

file(GLOB_RECURSE
    _resource_list
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/main/resources/mds/jscope/*"
)

add_jar(
    DeviceBeans
    SOURCES ${_source_list}
    RESOURCES ${_resource_list}
    INCLUDE_JARS mdsobjects jScope
    MANIFEST DeviceBeansManifest.mf
)