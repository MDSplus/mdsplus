
if(TARGET Motif::Motif)

    ###
    ### XmdsShr
    ###

    add_library(
        XmdsShr
        ListTree.c
        XmdsBells.c
        XmdsComplain.c
        XmdsDeviceSetup.c
        XmdsDigChans.c
        XmdsDisplay.c
        XmdsExpr.c
        XmdsGetPutNid.c
        XmdsGetSetOptionIdx.c
        XmdsInitialize.c
        XmdsInput.c
        XmdsInputs.c
        XmdsNidOptionMenu.c
        XmdsOkButtons.c
        XmdsOnOffToggleButton.c
        XmdsPath.c
        XmdsPopupMenuPosition.c
        XmdsSetSubvalues.c
        XmdsSupport.c
        XmdsUsageIcons.c
        XmdsWavedraw.c
        XmdsWaveform.c
        XmdsWidgetCallbacks.c
        XmdsXdBox.c
        XmdsXdBoxDialog.c
        XmdsXdBoxDialogButton.c
        XmdsXdBoxOnOffButton.c
    )

    target_link_libraries(
        XmdsShr
        PUBLIC
            TdiShr
            Motif::Motif
    )

    mdsplus_add_static_copy(XmdsShr _static_target)

    install(TARGETS XmdsShr ${_static_target})

    # ###
    # ### wmdcreate
    # ###

    # add_executable(
    #     wmdcreate
    #     wmldbcreate-xmdsshr.c
    # )

    # target_link_libraries(
    #     wmdcreate
    #     PUBLIC
    #         XmdsShr
    # )
    
endif()