
IF(IDL_FOUND)

    ###
    ### IdlMdsWidgets
    ###

    ADD_LIBRARY(
        IdlMdsWidgets SHARED
        cw_wveditv5.c
    )

    INSTALL(TARGETS IdlMdsWidgets)

    TARGET_LINK_LIBRARIES(
        IdlMdsWidgets
        PUBLIC
            MdsIpShr
            XmdsShr
            IDL::IDL
    )

    ADD_STATIC_COPY(IdlMdsWidgets)

ENDIF()