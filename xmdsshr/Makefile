include $(MAKE_OPTIONS)

CFLAGS = $(MOTIF_CFLAGS)

OBJECTS = \
ListTree.o \
XmdsBells.o \
XmdsComplain.o \
XmdsDeviceSetup.o \
XmdsDigChans.o \
XmdsDisplay.o \
XmdsExpr.o \
XmdsGetPutNid.o \
XmdsGetSetOptionIdx.o \
XmdsInitialize.o \
XmdsInput.o \
XmdsInputs.o \
XmdsNidOptionMenu.o \
XmdsOkButtons.o \
XmdsOnOffToggleButton.o \
XmdsPath.o \
XmdsPopupMenuPosition.o \
XmdsSetSubvalues.o \
XmdsSupport.o \
XmdsUsageIcons.o \
XmdsWavedraw.o \
XmdsWaveform.o \
XmdsWidgetCallbacks.o \
XmdsXdBox.o \
XmdsXdBoxDialog.o \
XmdsXdBoxDialogButton.o \
XmdsXdBoxOnOffButton.o

xmdsshr : ../shlib \
	../shlib/libXmdsShr.$(SHARE_TYPE) \
	../shlib/xmdsuil.wmd \
        ../uid \
	../uid/xmds_usage_icons.uid \
	../uid/XmdsDigChans.uid \
	../uid/XmdsInput.uid \
	../uid/XmdsXdBox.uid

../uid :
	mkdir ../uid

../shlib :
	mkdir ../shlib

../uid/xmds_usage_icons.uid : xmds_usage_icons.uil
	uil -o $@ xmds_usage_icons.uil

../uid/XmdsDigChans.uid : XmdsDigChans.uil
	uil -wmd ../shlib/xmdsuil.wmd -o $@ XmdsDigChans.uil

../uid/XmdsInput.uid : XmdsInput.uil
	uil -wmd ../shlib/xmdsuil.wmd -o $@ XmdsInput.uil

../uid/XmdsXdBox.uid : XmdsXdBox.uil
	uil -wmd ../shlib/xmdsuil.wmd -o $@ XmdsXdBox.uil

../shlib/libXmdsShr.$(SHARE_TYPE) : $(OBJECTS)
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -lc -lm \
           -L../shlib -lMdsShr -lTdiShr -lTreeShr $(MOTIF_LIBS) $(X11_LIBS)

../shlib/xmdsuil.wmd : $(WMD_RULE)

../bin/wmdcreate : wmldbcreate-xmdsshr.c UilConst.h
	cc -o $@ -DMDS_EXTENSION $(CFLAGS) $(WML_INCLUDE) wmldbcreate-xmdsshr.c $(MOTIF_LIBS) $(X11_LIBS) $(SOCKET_LIB)

UilConst.h : xmdsuil.wml 
	$(WML) xmdsuil.wml
	- rm wml-uil.mm
	- rm wml-uil.sdml

clean :
	- rm *.o
	- rm ../shlib/libXmdsShr.$(SHARE_TYPE)
	- rm .depend
	- rm ../shlib/xmdsuil.wmd

.depend :
	- rm .depend
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

