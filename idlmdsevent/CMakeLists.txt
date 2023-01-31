
if(IDL_FOUND)

    ###
    ### IdlMdsEvent
    ###

    add_library(
        IdlMdsEvent SHARED
        mdsevent.c
    )

    install(TARGETS IdlMdsEvent)

    target_link_libraries(
        IdlMdsEvent
        PUBLIC
            MdsIpShr
            Motif::Motif
            IDL::IDL
    )

    add_static_copy(IdlMdsEvent)

endif()