
IF(Motif_FOUND)

    ###
    ### XmdsShr
    ###

    ADD_LIBRARY(
        XmdsShr SHARED
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

    INSTALL(
        TARGETS XmdsShr
    )

    TARGET_LINK_LIBRARIES(
        XmdsShr
        PUBLIC
            TdiShr
            Motif::Motif
    )

    ADD_STATIC_COPY(XmdsShr)
    
ENDIF()