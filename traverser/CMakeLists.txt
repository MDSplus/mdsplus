
if(TARGET Motif::Motif)

    ###
    ### traverser
    ###

    add_executable(
        traverser
        main-uil.c
        bxutils-uil.c
        CallbacksUil.c
        ReadInt.c
        ReadString.c
        GetNciString.c
        GetSupportedDevices.c
    )

    target_link_libraries(
        traverser
        PRIVATE
            libMdsdcl
            XmdsShr
    )

    target_compile_definitions(
        traverser
        PRIVATE
            -D_NO_DXm
            -D_LOCAL_ACCESS
    )

    install(TARGETS traverser)

endif()