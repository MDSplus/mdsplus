
###
### UDT4
###

SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

FILE(GLOB
    _udt4_source_list
    "udt4/src/*.h"
    "udt4/src/*.cpp"
    "udt4/udtc/*.h"
    "udt4/udtc/*.cpp"
)

ADD_LIBRARY(
    UDT4 OBJECT
    ${_udt4_source_list}
)

TARGET_INCLUDE_DIRECTORIES(
    UDT4
    PUBLIC
        "udt4/src/"
        "udt4/udtc/"
)

SET_TARGET_PROPERTIES(
    UDT4
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

###
### compression
###

FILE(GLOB
    _compression_source_list
    "zlib/*.c"
)

ADD_LIBRARY(
    compression OBJECT
    ${_compression_source_list}
)

TARGET_INCLUDE_DIRECTORIES(
    compression
    PUBLIC
        "zlib/"
)

SET_TARGET_PROPERTIES(
    compression
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

###
### MdsIpShr
###

FILE(GLOB
    _mdsipshr_source_list
    "mdsipshr/*.c"
)

ADD_LIBRARY(
    MdsIpShr SHARED
    ${_mdsipshr_source_list}
    io_routines/IoRoutinesTunnel.c
    io_routines/IoRoutinesThread.c
)

INSTALL(TARGETS MdsIpShr)

TARGET_LINK_LIBRARIES(
    MdsIpShr
    PUBLIC
        TdiShr
        compression
)

ADD_STATIC_COPY(MdsIpShr)

###
### MdsIpTCP / MdsIpTCPV6
###

ADD_LIBRARY(
    MdsIpTCP SHARED
    io_routines/IoRoutinesTcp.c
)

INSTALL(TARGETS MdsIpTCP)

ADD_LIBRARY(
    MdsIpTCPV6 SHARED
    io_routines/IoRoutinesTcpV6.c
)

INSTALL(TARGETS MdsIpTCPV6)

###
### MdsIpUDT / MdsIpUDTV6
###

ADD_LIBRARY(
    MdsIpUDT SHARED
    io_routines/IoRoutinesUdt.c
)

INSTALL(TARGETS MdsIpUDT)

TARGET_LINK_LIBRARIES(
    MdsIpUDT
    PRIVATE
        UDT4
)

ADD_LIBRARY(
    MdsIpUDTV6 SHARED
    io_routines/IoRoutinesUdtV6.c
)

INSTALL(TARGETS MdsIpUDTV6)

TARGET_LINK_LIBRARIES(
    MdsIpUDTV6
    PRIVATE
        UDT4
)

IF(Globus_FOUND AND Globus_FLAVOR)

    ###
    ### MdsIpGSI
    ###

    ADD_LIBRARY(
        MdsIpGSI SHARED
        io_routines/IoRoutinesGsi.c 
    )

    INSTALL(TARGETS MdsIpGSI)

    TARGET_LINK_LIBRARIES(
        MdsIpGSI
        PUBLIC
            Globus::Globus
    )

ENDIF()

###
### mdsip
###

ADD_EXECUTABLE(
    mdsip
    mdsip.c
)

INSTALL(TARGETS mdsip)

TARGET_LINK_LIBRARIES(
    mdsip
    PUBLIC
        MdsIpShr
)

###
### mdsip_service
###

IF(WIN32)
    ADD_EXECUTABLE(
        mdsip_service
        mdsip_service.c
    )

    INSTALL(TARGETS mdsip_service)

    TARGET_LINK_LIBRARIES(
        mdsip_service
        PUBLIC
            MdsIpShr
    )
ENDIF()

###
### mdsip.hosts / multi.hosts
###

IF(WIN32)
    SET(_win ".win")
ENDIF()

INSTALL(
    FILES mdsip.hosts RENAME mdsip.hosts${_win}
    DESTINATION etc 
)

INSTALL(
    FILES multi.hosts RENAME multi.hosts${_win}
    DESTINATION etc 
)

###
### Scripts
###

IF(WIN32)
    INSTALL(
        FILES
            # mdsip_service.exe.manifest # TODO: ???
            mdsip-client-ssh.bat
            mdsip-client-sshp.bat
            mdsip-client-local.bat
        TYPE BIN
    )
ELSE()
    INSTALL(
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

ADD_CUSTOM_TARGET(
    mdstcpip-docs
    COMMAND ${DOXYGEN_PROGRAM} docs/doxy.conf
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_DEPENDENCIES(
    docs mdstcpip-docs
)