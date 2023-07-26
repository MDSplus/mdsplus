
if(ENABLE_IDL AND IDL_FOUND)

    ###
    ### IdlMdsWidgets
    ###

    add_library(
        IdlMdsWidgets
        cw_wveditv5.c
    )

    target_link_libraries(
        IdlMdsWidgets
        PUBLIC
            MdsIpShr
            MdsMath
            XmdsShr
            IDL::IDL
    )

    add_static_copy(IdlMdsWidgets)

    install(TARGETS IdlMdsWidgets IdlMdsWidgets-static)

endif()