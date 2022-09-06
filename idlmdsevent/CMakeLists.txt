
IF(IDL_FOUND)

    ###
    ### IdlMdsEvent
    ###

    ADD_LIBRARY(
        IdlMdsEvent SHARED
        mdsevent.c
    )

    INSTALL(TARGETS IdlMdsEvent)

    TARGET_LINK_LIBRARIES(
        IdlMdsEvent
        PUBLIC
            MdsIpShr
            Motif::Motif
            IDL::IDL
    )

    ADD_STATIC_COPY(IdlMdsEvent)

ENDIF()