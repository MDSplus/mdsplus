
if(Sybase_FOUND)

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

    mdsplus_add_static_copy(MdsSql)

    install(TARGETS MdsSql MdsSql-static)

endif()