
###
### MdsServerShr
###

add_library(
    MdsServerShr
    Client.h
    Job.h
    ServerAbortServer.c
    ServerBuildDispatchTable.c
    ServerCloseTrees.c
    ServerCreatePulse.c
    ServerDispatchAction.c
    ServerDispatchClose.c
    ServerDispatchCommand.c
    ServerDispatchPhase.c
    ServerFindServers.c
    ServerGetInfo.c
    ServerMonitorCheckin.c
    ServerQAction.c
    ServerSendMessage.c
    ServerSendMonitor.c
    ServerSetDetailProc.c
    ServerSetLogging.c
    servershrp.h
    ServerStartServer.c
    ServerStopServer.c
)

target_link_libraries(
    MdsServerShr
    PUBLIC
        libMdsdcl
)

mdsplus_add_static_copy(MdsServerShr _static_target)

install(TARGETS MdsServerShr ${_static_target})