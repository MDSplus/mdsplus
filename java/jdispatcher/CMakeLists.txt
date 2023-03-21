
###
### jDispatcher.jar
###


###
### Scripts
###

if(WIN32)
    install(
        FILES
            jDispatcherIp.bat
            jDispatchMonitor.bat
            jServer.bat
        TYPE BIN
    )
else()
    install(
        FILES jDispatcherIp.template RENAME jDispatcherIp
        TYPE BIN
        PERMISSIONS ${MODE_755}
    )
    install(
        FILES jDispatchMonitor.template RENAME jDispatchMonitor
        TYPE BIN
        PERMISSIONS ${MODE_755}
    )
    install(
        FILES jServer.template RENAME jServer
        TYPE BIN
        PERMISSIONS ${MODE_755}
    )
endif()