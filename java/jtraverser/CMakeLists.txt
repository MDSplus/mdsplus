
###
### jTraverser.jar
###

###
### Scripts
###

if(WIN32)
    install(
        FILES
            jTraverser.bat
        TYPE BIN
    )
else()
    install(
        FILES jTraverser.template RENAME jTraverser
        TYPE BIN
    )
    install(
        FILES CompileTree.template RENAME CompileTree
        TYPE BIN
    )
    install(
        FILES DecompileTree.template RENAME DecompileTree
        TYPE BIN
    )
endif()