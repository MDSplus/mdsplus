
if(TARGET IDL::IDL AND TARGET Motif::Motif)

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
            IDL::IDL
            Motif::Motif
    )

    mdsplus_add_static_copy(IdlMdsEvent _static_target)

    install(TARGETS IdlMdsEvent ${_static_target})

endif()