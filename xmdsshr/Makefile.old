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

EXPORTS = xmdsshr.export

xmdsshr : ../shlib \
	../shlib/xmdsuil.wmd \
        ../uid \
	../uid/xmds_usage_icons.uid \
	../uid/XmdsDigChans.uid \
	../uid/XmdsInput.uid \
	../uid/XmdsXdBox.uid \
	../shlib/libXmdsShr.a \
	../shlib/libXmdsShr.$(SHARE_TYPE)

../uid :
	mkdir ../uid

../shlib :
	mkdir ../shlib

../uid/xmds_usage_icons.uid : xmds_usage_icons.uil
	uil -o $@ xmds_usage_icons.uil

../uid/XmdsDigChans.uid : XmdsDigChans.uil
	uil -o $@ XmdsDigChans.uil

../uid/XmdsInput.uid : XmdsInput.uil
	uil -o $@ XmdsInput.uil

../uid/XmdsXdBox.uid : XmdsXdBox.uil
	uil -o $@ XmdsXdBox.uil

../shlib/libXmdsShr.$(SHARE_TYPE) : $(OBJECTS)
	- $(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -lc -lm \
           -L../shlib -lMdsShr -lTdiShr -lTreeShr $(MOTIF_LIBS) $(X11_LIBS)

../shlib/libXmdsShr.a : $(OBJECTS)
	$(AR) -r $@ $(OBJECTS)

../shlib/xmdsuil.wmd : $(WMD_RULE)

../bin/wmdcreate : wmldbcreate-xmdsshr.c UilConst.h
	$(CC) -o $@ -DMDS_EXTENSION $(CFLAGS) $(WML_INCLUDE) wmldbcreate-xmdsshr.c $(MOTIF_LIBS) $(X11_LIBS) $(SOCKET_LIB)

UilConst.h : xmdsuil.wml 
	$(WML) 

clean :
	- rm *.o
	- rm ../bin/wmdcreate
	- rm ../shlib/libXmdsShr.a
	- rm ../shlib/libXmdsShr.$(SHARE_TYPE)
	- rm .depend
	- rm ../shlib/xmdsuil.wmd
	- rm ../uid/xmds_usage_icons.uid
	- rm ../uid/XmdsDigChans.uid
	- rm ../uid/XmdsInput.uid
	- rm ../uid/XmdsXdBox.uid

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

