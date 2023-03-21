
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
        PERMISSIONS ${MODE_755}
    )
    install(
        FILES CompileTree.template RENAME CompileTree
        TYPE BIN
        PERMISSIONS ${MODE_755}
    )
    install(
        FILES DecompileTree.template RENAME DecompileTree
        TYPE BIN
        PERMISSIONS ${MODE_755}
    )
endif()