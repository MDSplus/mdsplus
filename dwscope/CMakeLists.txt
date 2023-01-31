
IF(Motif_FOUND)

    add_definitions(
        -D_NO_DXm
    )

    ###
    ### dwscope
    ###

    add_executable(
        dwscope
        dwscope.c
        dwscopesubs.c
        evaluate.c
    )

    install(TARGETS dwscope)

    target_link_libraries(
        dwscope
        PRIVATE
            XmdsShr
    )

    target_compile_definitions(
        dwscope
        PRIVATE
            -D_LOCAL_ACCESS
    )

    ###
    ### dwscope_remote
    ###

    add_executable(
        dwscope_remote
        dwscope.c
        dwscopesubs.c
        evaluate.c
        RemoteDummies.c
    )

    install(TARGETS dwscope_remote)

    target_link_libraries(
        dwscope_remote
        PRIVATE
            XmdsShr-static
            MdsIpShr
    )

    ###
    ### dwpad
    ###

    add_executable(
        dwpad
        dwpad.c
        dwscopesubs.c
    )

    install(TARGETS dwpad)

    target_link_libraries(
        dwpad
        PRIVATE
            XmdsShr
    )

    ###
    ### Scripts
    ###

    install(
        FILES
            ScopePrinters
            dwscopePrint
        PERMISSIONS ${MODE_755}
        TYPE BIN
    )

    ###
    ### Resources
    ###

    install(
        FILES dwscope_setup.ps
        PERMISSIONS ${MODE_644}
        TYPE LIB
    )

ENDIF()