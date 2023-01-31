
# (SLW) TODO
if(VMS)

    ###
    ### MdsVme
    ###

    add_library(
        MdsVme
        mdsvme.c
    )

    target_link_libraries(
        MdsVme
        PRIVATE
            ${LIBDL}
    )

endif()