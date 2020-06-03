@start "" javaw -Xss5M -cp ^
"%MDSPLUS_DIR%\java\classes\jTraverser.jar^
;%MDSPLUS_DIR%\java\classes\mdsobjects.jar^
;%MDSPLUS_DIR%\java\classes\jScope.jar^
;%MDSPLUS_DIR%\java\classes\DeviceBeans.jar^
;%MDSPLUS_DIR%\java\classes\jDevices.jar^
" jTraverser %*
