
###
### actlog
###

ADD_EXECUTABLE(
    actlog
    actlog.c
)

INSTALL(TARGETS actlog)

TARGET_LINK_LIBRARIES(
    actlog
    PRIVATE
        MdsServerShr
)

IF(Motif_FOUND)

    ###
    ### actions
    ###

    ADD_EXECUTABLE(
        actions
        actions.c
    )

    INSTALL(TARGETS actions)

    TARGET_LINK_LIBRARIES(
        actions
        PRIVATE
            XmdsShr
    )

    ###
    ### actmon
    ###

    ADD_EXECUTABLE(
        actmon
        actmon.c
    )

    INSTALL(TARGETS actmon)

    IF(WIN32)
        TARGET_SOURCES(
            actmon
            PRIVATE
                icon.rc
        )
    ENDIF()

    TARGET_LINK_LIBRARIES(
        actmon
        PRIVATE
            Motif::Motif
            MdsServerShr
    )
    
ENDIF()