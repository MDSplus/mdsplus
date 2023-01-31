
if(IDL_FOUND)

    ###
    ### IdlMdsWidgets
    ###

    add_library(
        IdlMdsWidgets SHARED
        cw_wveditv5.c
    )

    install(TARGETS IdlMdsWidgets)

    target_link_libraries(
        IdlMdsWidgets
        PUBLIC
            MdsIpShr
            XmdsShr
            IDL::IDL
    )

    add_static_copy(IdlMdsWidgets)

endif()