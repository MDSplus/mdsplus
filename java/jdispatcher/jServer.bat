@start "" javaw -Xss10M -cp ^
"%MDSPLUS_DIR%\java\classes\jDispatcher.jar^
;%MDSPLUS_DIR%\java\classes\jTraverser.jar^
;%MDSPLUS_DIR%\java\classes\mdsobjects.jar^
;%MDSPLUS_DIR%\java\classes\WaveDisplay.jar^
" mds.jdispatcher.jServer %*
