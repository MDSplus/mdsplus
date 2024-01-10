
if(TARGET Sybase::Sybase)

    ###
    ### MdsSql
    ###

    add_library(
        MdsSql
        dynamic_syb.c
        dbstubs.c
    )

    target_link_libraries(
        MdsSql
        PUBLIC
            Sybase::Sybase
    )

    mdsplus_add_static_copy(MdsSql _static_target)

    install(TARGETS MdsSql ${_static_target})

endif()