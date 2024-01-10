
###
### jDispatcher.jar
###


###
### Scripts
###

if(WIN32)
    set(_script_list
        jDispatcherIp.bat
        jDispatchMonitor.bat
        jServer.bat
    )
else()
    set(_script_list
        jDispatcherIp
        jDispatchMonitor
        jServer
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