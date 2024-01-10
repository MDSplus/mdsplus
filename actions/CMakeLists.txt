
###
### actlog
###

add_executable(
    actlog
    actlog.c
)

target_link_libraries(
    actlog
    PRIVATE
        MdsServerShr
)

install(TARGETS actlog)

if(TARGET XmdsShr)

    ###
    ### actions
    ###

    add_executable(
        actions
        actions.c
    )

    target_link_libraries(
        actions
        PRIVATE
            XmdsShr
    )

    install(TARGETS actions)

endif()

if(TARGET Motif::Motif)

    ###
    ### actmon
    ###

    add_executable(
        actmon
        actmon.c
    )

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

    install(TARGETS actmon)
    
endif()