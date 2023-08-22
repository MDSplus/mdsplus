
###
### jTraverser.jar
###

list(APPEND CMAKE_JAVA_COMPILE_FLAGS -source 8)

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