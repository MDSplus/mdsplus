
IF(Motif_FOUND)

    ###
    ### traverser
    ###

    ADD_EXECUTABLE(
        traverser
        main-uil.c
        bxutils-uil.c
        CallbacksUil.c
        ReadInt.c
        ReadString.c
        GetNciString.c
        GetSupportedDevices.c
    )

    INSTALL(TARGETS traverser)

    TARGET_LINK_LIBRARIES(
        traverser
        PRIVATE
            Mdsdcl
            XmdsShr
    )

    TARGET_COMPILE_DEFINITIONS(
        traverser
        PRIVATE
            -D_NO_DXm
            -D_LOCAL_ACCESS
    )

ENDIF()