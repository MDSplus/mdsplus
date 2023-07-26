
if(ENABLE_IDL AND IDL_FOUND)

    ###
    ### IdlMdsEvent
    ###

    add_library(
        IdlMdsEvent
        mdsevent.c
    )

    target_link_libraries(
        IdlMdsEvent
        PUBLIC
            MdsIpShr
            Motif::Motif
            IDL::IDL
    )

    add_static_copy(IdlMdsEvent)

    install(TARGETS IdlMdsEvent IdlMdsEvent-static)

endif()