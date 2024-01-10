
IF(TARGET XmdsShr)

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

    install(TARGETS dwscope)

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

    target_link_libraries(
        dwscope_remote
        PRIVATE
            XmdsShr-static
            MdsIpShr
    )

    install(TARGETS dwscope_remote)

    ###
    ### dwpad
    ###

    add_executable(
        dwpad
        dwpad.c
        dwscopesubs.c
    )

    target_link_libraries(
        dwpad
        PRIVATE
            XmdsShr
    )

    install(TARGETS dwpad)

    ###
    ### Scripts
    ###

    set(_script_list
        ScopePrinters
        dwscopePrint
    )

    file(COPY ${_script_list}
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

    install(
        FILES ${_script_list}
        TYPE BIN
        PERMISSIONS ${MODE_755}
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