
###
### actlog
###

add_executable(
    actlog
    actlog.c
)

install(TARGETS actlog)

target_link_libraries(
    actlog
    PRIVATE
        MdsServerShr
)

if(Motif_FOUND)

    ###
    ### actions
    ###

    add_executable(
        actions
        actions.c
    )

    install(TARGETS actions)

    target_link_libraries(
        actions
        PRIVATE
            XmdsShr
    )

    ###
    ### actmon
    ###

    add_executable(
        actmon
        actmon.c
    )

    install(TARGETS actmon)

    if(WIN32)
        target_sources(
            actmon
            PRIVATE
                icon.rc
        )
    endif()

    target_link_libraries(
        actmon
        PRIVATE
            Motif::Motif
            MdsServerShr
    )
    
endif()