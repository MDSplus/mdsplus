
###
### jTraverser.jar
###

###
### Scripts
###

IF(WIN32)
    INSTALL(
        FILES
            jTraverser.bat
        TYPE BIN
    )
ELSE()
    INSTALL(
        FILES jTraverser.template RENAME jTraverser
        TYPE BIN
    )
    INSTALL(
        FILES CompileTree.template RENAME CompileTree
        TYPE BIN
    )
    INSTALL(
        FILES DecompileTree.template RENAME DecompileTree
        TYPE BIN
    )
ENDIF()