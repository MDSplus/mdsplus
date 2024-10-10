
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
        ZLIB::ZLIB
)

target_compile_definitions(
    MdsIpShr
    PRIVATE
        -DLIBPREFIX=MdsIp
)

mdsplus_add_static_copy(MdsIpShr _static_target)

install(TARGETS MdsIpShr ${_static_target})

###
### MdsIpTCP
###

add_library(
    MdsIpTCP
    io_routines/IoRoutinesTcp.c
)

target_link_libraries(
    MdsIpTCP
    PRIVATE
        MdsIpShr
)

install(TARGETS MdsIpTCP)

if(NOT WIN32)

    ###
    ### MdsIpTCPV6
    ###

    add_library(
        MdsIpTCPV6
        io_routines/IoRoutinesTcpV6.c
    )

    target_link_libraries(
        MdsIpTCPV6
        PRIVATE
            MdsIpShr
    )

    install(TARGETS MdsIpTCPV6)

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

    # TODO: Check if needed
    target_compile_options(
        UDT4
        PRIVATE
        -Wno-null-pointer-subtraction
    )
    
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
            MdsIpShr
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
            MdsIpShr
            UDT4
    )

    install(TARGETS MdsIpUDTV6)

endif()

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
endif()

install(
    FILES mdsip.hosts${_win} RENAME mdsip.hosts
    TYPE SYSCONF
)

install(
    FILES multi.hosts${_win} RENAME multi.hosts
    TYPE SYSCONF
)

###
### Scripts
###

if(WIN32)
    set(_script_list
        # mdsip_service.exe.manifest # TODO: ???
        mdsip-client-ssh.bat
        mdsip-client-sshp.bat
        mdsip-client-local.bat
    )
else()
    set(_script_list
        mdsipd
        mdsip_server
        mdsip-client-ssh
        mdsip-client-sshp
        mdsip-client-http
        mdsip-client-local
        mdsip-server-ssh
        mdsip-server-http
    )
endif()

if(TARGET Globus::Globus)

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
        PRIVATE
            MdsIpShr
    )

    install(TARGETS MdsIpGSI)

    ###
    ### Globus-only Scripts
    ###
    
    list(APPEND _script_list
        mdsipsd
    )

endif()

file(COPY ${_script_list}
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

install(
    FILES ${_script_list}
    TYPE BIN
    PERMISSIONS ${MODE_755}
    
)

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