
###
### UDT4
###

# TODO: Investigate?
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

target_link_libraries(
    MdsIpShr
    PUBLIC
        TdiShr
        compression
)

target_compile_definitions(
    MdsIpShr
    PRIVATE
        -DLIBPREFIX=MdsIp
)

add_static_copy(MdsIpShr)

install(TARGETS MdsIpShr MdsIpShr-static)

###
### MdsIpTCP
###

add_library(
    MdsIpTCP
    io_routines/IoRoutinesTcp.c
)

target_link_libraries(
    MdsIpTCP
    PUBLIC
        MdsIpShr
)

install(TARGETS MdsIpTCP)

###
### MdsIpTCPV6
###

add_library(
    MdsIpTCPV6
    io_routines/IoRoutinesTcpV6.c
)

target_link_libraries(
    MdsIpTCPV6
    PUBLIC
        MdsIpShr
)

install(TARGETS MdsIpTCPV6)

###
### MdsIpUDT
###

add_library(
    MdsIpUDT
    io_routines/IoRoutinesUdt.c
)

target_link_libraries(
    MdsIpUDT
    PUBLIC
        MdsIpShr
    PRIVATE
        UDT4
)

install(TARGETS MdsIpUDT)

###
### MdsIpUDTV6
###

add_library(
    MdsIpUDTV6
    io_routines/IoRoutinesUdtV6.c
)

target_link_libraries(
    MdsIpUDTV6
    PUBLIC
        MdsIpShr
    PRIVATE
        UDT4
)

install(TARGETS MdsIpUDTV6)

###
### mdsip
###

add_executable(
    mdsip
    mdsip.c
)

target_link_libraries(
    mdsip
    PUBLIC
        MdsIpShr
)

install(TARGETS mdsip)

if(WIN32)

    ###
    ### mdsip_service
    ###

    add_executable(
        mdsip_service
        mdsip_service.c
    )

    target_link_libraries(
        mdsip_service
        PUBLIC
            MdsIpShr
    )

    install(TARGETS mdsip_service)

endif()

###
### mdsip.hosts / multi.hosts
###

if(WIN32)
    SET(_win ".win")
ENDIF()

install(
    FILES mdsip.hosts RENAME mdsip.hosts${_win}
    TYPE SYSCONF
)

install(
    FILES multi.hosts RENAME multi.hosts${_win}
    TYPE SYSCONF
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
            mdsip_server
            mdsip-client-ssh
            mdsip-client-sshp
            mdsip-client-http
            mdsip-client-local
            mdsip-server-ssh
            mdsip-server-http
        TYPE BIN
        PERMISSIONS ${MODE_755}
    )
ENDIF()

if(Globus_FOUND AND Globus_FLAVOR)

    ###
    ### MdsIpGSI
    ###

    add_library(
        MdsIpGSI
        io_routines/IoRoutinesGsi.c 
    )

    target_link_libraries(
        MdsIpGSI
        PUBLIC
            MdsIpShr
            Globus::Globus
    )

    install(TARGETS MdsIpGSI)

    ###
    ### Globus-only Scripts
    ###
    
    install(
        FILES mdsipsd
        TYPE BIN
        PERMISSIONS ${MODE_755}
    )

endif()

if(Doxygen_FOUND)
    ###
    ### Documentation
    ###

    add_custom_target(
        mdstcpip-docs
        COMMAND ${DOXYGEN_EXECUTABLE} docs/doxy.conf
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_dependencies(
        docs mdstcpip-docs
    )
endif()