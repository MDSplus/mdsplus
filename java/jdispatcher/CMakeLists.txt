
###
### jDispatcher.jar
###

###
### Scripts
###

IF(WIN32)
    INSTALL(
        FILES
            jDispatcherIp.bat
            jDispatchMonitor.bat
            jServer.bat
        TYPE BIN
    )
ELSE()
    INSTALL(
        FILES jDispatcherIp.template RENAME jDispatcherIp
        TYPE BIN
    )
    INSTALL(
        FILES jDispatchMonitor.template RENAME jDispatchMonitor
        TYPE BIN
    )
    INSTALL(
        FILES jServer.template RENAME jServer
        TYPE BIN
    )
ENDIF()