
###
### UDT4
###

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

file(GLOB
    _udt4_source_list
    "udt4/src/*.h"
    "udt4/src/*.cpp"
    "udt4/udtc/*.h"
    "udt4/udtc/*.cpp"
)

add_library(
    UDT4 OBJECT
    ${_udt4_source_list}
)

target_include_directories(
    UDT4
    PUBLIC
        "udt4/src/"
        "udt4/udtc/"
)

set_target_properties(
    UDT4
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

###
### compression
###

file(GLOB
    _compression_source_list
    "zlib/*.c"
)

add_library(
    compression OBJECT
    ${_compression_source_list}
)

target_include_directories(
    compression
    PUBLIC
        "zlib/"
)

set_target_properties(
    compression
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

###
### MdsIpShr
###

file(GLOB
    _mdsipshr_source_list
    "mdsipshr/*.c"
)

add_library(
    MdsIpShr
    ${_mdsipshr_source_list}
    io_routines/IoRoutinesTunnel.c
    io_routines/IoRoutinesThread.c
)

install(TARGETS MdsIpShr)

target_link_libraries(
    MdsIpShr
    PUBLIC
        TdiShr
        compression
)

add_static_copy(MdsIpShr)

###
### MdsIpTCP / MdsIpTCPV6
###

add_library(
    MdsIpTCP
    io_routines/IoRoutinesTcp.c
)

install(TARGETS MdsIpTCP)

add_library(
    MdsIpTCPV6
    io_routines/IoRoutinesTcpV6.c
)

install(TARGETS MdsIpTCPV6)

###
### MdsIpUDT / MdsIpUDTV6
###

add_library(
    MdsIpUDT
    io_routines/IoRoutinesUdt.c
)

install(TARGETS MdsIpUDT)

target_link_libraries(
    MdsIpUDT
    PRIVATE
        UDT4
)

add_library(
    MdsIpUDTV6
    io_routines/IoRoutinesUdtV6.c
)

install(TARGETS MdsIpUDTV6)

target_link_libraries(
    MdsIpUDTV6
    PRIVATE
        UDT4
)

if(Globus_FOUND AND Globus_FLAVOR)

    ###
    ### MdsIpGSI
    ###

    add_library(
        MdsIpGSI
        io_routines/IoRoutinesGsi.c 
    )

    install(TARGETS MdsIpGSI)

    target_link_libraries(
        MdsIpGSI
        PUBLIC
            Globus::Globus
    )

endif()

###
### mdsip
###

add_executable(
    mdsip
    mdsip.c
)

install(TARGETS mdsip)

target_link_libraries(
    mdsip
    PUBLIC
        MdsIpShr
)

###
### mdsip_service
###

if(WIN32)
    add_executable(
        mdsip_service
        mdsip_service.c
    )

    install(TARGETS mdsip_service)

    target_link_libraries(
        mdsip_service
        PUBLIC
            MdsIpShr
    )
endif()

###
### mdsip.hosts / multi.hosts
###

if(WIN32)
    SET(_win ".win")
ENDIF()

install(
    FILES mdsip.hosts RENAME mdsip.hosts${_win}
    DESTINATION etc 
)

install(
    FILES multi.hosts RENAME multi.hosts${_win}
    DESTINATION etc 
)

###
### Scripts
###

if(WIN32)
    install(
        FILES
            # mdsip_service.exe.manifest # TODO: ???
            mdsip-client-ssh.bat
            mdsip-client-sshp.bat
            mdsip-client-local.bat
        TYPE BIN
    )
else()
    install(
        FILES
            mdsipd
            mdsipsd
            mdsip_server
            mdsip-client-ssh
            mdsip-client-sshp
            mdsip-client-http
            mdsip-client-local
            mdsip-server-ssh
            mdsip-server-http
        TYPE BIN
    )
ENDIF()

###
### Documentation
###

add_custom_target(
    mdstcpip-docs
    COMMAND ${doxygen_EXECUTABLE} docs/doxy.conf
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_dependencies(
    docs mdstcpip-docs
)