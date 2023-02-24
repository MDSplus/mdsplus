
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
    )
    install(
        FILES jDispatchMonitor.template RENAME jDispatchMonitor
        TYPE BIN
    )
    install(
        FILES jServer.template RENAME jServer
        TYPE BIN
    )
endif()