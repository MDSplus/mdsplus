# Global variables
AM_CFLAGS = $(X_CFLAGS)
LDADD = -L@MAKESHLIBDIR@ -lXmdsShr $(X_PRE_LIBS) $(MOTIF_LIBS) $(X_LIBS) $(X_EXTRA_LIBS)


#MOTIF_LIBS = @XM_LIBS@ @MOTIF_LDSHARE@ @LIBXP@ @LIBXEXT@
MOTIF_LIBS = -lMrm -lXm -lXt -lX11

#FIXME: Remove this when LIBS is cleaned up in configure
LIBS=

# Static data installed to lib
datadir = $(libdir)
data_DATA = dwscope_setup.ps

# Scripts that are copied directly
bin_SCRIPTS = ScopePrinters dwscopePrint

# Helper libs
noinst_LIBRARIES = libXmdsWC.a
libXmdsWC_a_SOURCES = ../xmdsshr/XmdsWidgetCallbacks.c
libXmdsWC_a_CPPFLAGS = $(AM_CPPFLAGS) -D_NO_XDS

# dw programs
bin_PROGRAMS = dwscope
dwscope_SOURCES = dwscope.c dwscopesubs.c evaluate.c
dwscope_CPPFLAGS = $(AM_CPPFLAGS) -D_LOCAL_ACCESS
dwscope_LDADD = $(LDADD) -lMdsShr -lTdiShr -lTreeShr
dwscope_LDFLAGS = 
#$(MLIBS)

bin_PROGRAMS += dwscope_remote
dwscope_remote_SOURCES = $(dwscope_SOURCES) $(XMDSSRC) RemoteDummies.c
dwscope_remote_LDADD = $(LDADD) libXmdsWC.a -lMdsIpShr
dwscope_remote_LDFLAGS = $(MLIBS)

bin_PROGRAMS += dwpad
dwpad_SOURCES = dwpad.c dwscopesubs.c $(XMDSSRC)
dwpad_LDADD = libXmdsWC.a
dwpad_LDFLAGS = $(MLIBS)

# User interface descriptions, installed as generic data but with build rules
uiddir = @MAKEUIDDIR@
uid_DATA = dwscope.uid dwpad.uid
%.uid: %.uil
	@UIL@ -I$(top_srcdir)/include -o $@ $<


#@MAKEBINDIR@dwscope : dwscope.o dwscopesubs.o evaluate_local.o
#	$(CC) -o $@ $(CFLAGS) @CC_LDFLAGS@ dwscope.o dwscopesubs.o  evaluate_local.o -L@MAKESHLIBDIR@ -lTdiShr -lTreeShr -lMdsShr -lXmdsShr $(MLIBS)
                        
#@MAKEBINDIR@dwscope_remote : dwscope.o dwscopesubs.o evaluate.o RemoteDummies.o 
#	$(CC) -o $@ $(CFLAGS) @CC_LDFLAGS@ dwscope.o  dwscopesubs.o evaluate.o RemoteDummies.o XmdsWidgetCallbacks.o \
#              -L@MAKELIBDIR@ @LDARC@ -lXmdsShr @LDSHARE@ -L@MAKESHLIBDIR@ -lMdsIpShr -lMdsShr $(MLIBS) @LIBM@ -L@MAKESHLIBDIR@ @MEMCHKLIB@

