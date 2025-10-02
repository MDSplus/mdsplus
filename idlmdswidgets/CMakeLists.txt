
if(TARGET IDL::IDL AND TARGET XmdsShr)

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

    mdsplus_add_static_copy(IdlMdsWidgets _static_target)

    install(TARGETS IdlMdsWidgets ${_static_target})

endif()