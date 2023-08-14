
###
### taptest.jar
###

file(GLOB
    _jar_dependencies
    "*.jar"
)

add_jar(
    taptest
    SOURCES
        org/junit/runner/TapTestRunner.java
    INCLUDE_JARS ${_jar_dependencies}
)
