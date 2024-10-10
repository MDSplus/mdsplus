
###
### MdsShr
###

add_library(
    MdsShr
    gzipcompress.c
    librtl.c
    mds_dsc_string.c
    MdsCmprs.c
    MdsCompareXd.c
    MdsCompress.c
    MdsEvents.c
    MdsGet1DxA.c
    MdsGet1DxS.c
    MdsGetSetShotId.c
    MdsGetStdMsg.c
    mdsmsg.c
    MdsPk.c
    MDSprintf.c
    MdsSandbox.c
    MdsSerialize.c
    MdsThreadStatic.c
    MdsXdRoutines.c
    UdpEvents.c
    UdpEventSettings.c
)

target_link_libraries(
    MdsShr
    PUBLIC
        ${CMAKE_DL_LIBS}
        Iconv::Iconv
        LibM::LibM
        LibXml2::LibXml2
        Threads::Threads
        ZLIB::ZLIB
)

if(TARGET LibLZMA::LibLZMA)
    target_link_libraries(
        MdsShr
        PUBLIC
            LibLZMA::LibLZMA
    )
endif()

target_compile_definitions(
    MdsShr
    PRIVATE
        -DLIBPREFIX=Mds
)

if(WIN32)
    TARGET_LINK_LIBRARIES(
        MdsShr
        PUBLIC
            iphlpapi
            ws2_32
    )
endif()

mdsplus_add_static_copy(MdsShr _static_target)

install(TARGETS MdsShr ${_static_target})

if(Doxygen_FOUND)
    ###
    ### Documentation
    ###

    add_custom_target(
        mdsshr-docs
        COMMAND ${DOXYGEN_EXECUTABLE} docs/doxy.conf
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_dependencies(
        docs mdsshr-docs
    )
endif()