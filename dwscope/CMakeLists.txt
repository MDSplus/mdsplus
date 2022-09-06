
IF(Motif_FOUND)

    ADD_DEFINITIONS(
        -D_NO_DXm
    )

    ###
    ### dwscope
    ###

    ADD_EXECUTABLE(
        dwscope
        dwscope.c
        dwscopesubs.c
        evaluate.c
    )

    INSTALL(TARGETS dwscope)

    TARGET_LINK_LIBRARIES(
        dwscope
        PRIVATE
            XmdsShr
    )

    TARGET_COMPILE_DEFINITIONS(
        dwscope
        PRIVATE
            -D_LOCAL_ACCESS
    )

    ###
    ### dwscope_remote
    ###

    ADD_EXECUTABLE(
        dwscope_remote
        dwscope.c
        dwscopesubs.c
        evaluate.c
        RemoteDummies.c
    )

    INSTALL(TARGETS dwscope_remote)

    TARGET_LINK_LIBRARIES(
        dwscope_remote
        PRIVATE
            XmdsShr-static
            MdsIpShr
    )

    ###
    ### dwpad
    ###

    ADD_EXECUTABLE(
        dwpad
        dwpad.c
        dwscopesubs.c
    )

    INSTALL(TARGETS dwpad)

    TARGET_LINK_LIBRARIES(
        dwpad
        PRIVATE
            XmdsShr
    )

    ###
    ### Scripts
    ###

    INSTALL(
        FILES ScopePrinters dwscopePrint
        PERMISSIONS ${MODE_755}
        TYPE BIN
    )

    ###
    ### Resources
    ###

    INSTALL(
        FILES dwscope_setup.ps
        PERMISSIONS ${MODE_644}
        TYPE LIB
    )

ENDIF()