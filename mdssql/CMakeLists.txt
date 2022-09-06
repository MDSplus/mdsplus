
if(Sybase_FOUND)

    ###
    ### MdsSql
    ###

    add_library(
        MdsSql
        dynamic_syb.c
        dbstubs.c
    )

    install(TARGETS MdsSql)

    target_link_libraries(
        MdsSql
        PUBLIC
            Sybase::Sybase
    )

    add_static_copy(MdsSql)

endif()