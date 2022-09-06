
IF(Sybase_FOUND)

    ###
    ### MdsSql
    ###

    ADD_LIBRARY(
        MdsSql SHARED
        dynamic_syb.c
        dbstubs.c
    )

    INSTALL(TARGETS MdsSql)

    TARGET_LINK_LIBRARIES(
        MdsSql
        PUBLIC
            Sybase::Sybase
    )

    ADD_STATIC_COPY(MdsSql)

ENDIF()