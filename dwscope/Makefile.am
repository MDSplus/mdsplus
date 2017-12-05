# Global variables
AM_CFLAGS = $(TARGET_ARCH) $(WARNFLAGS) $(NOWARNFLAGS)

#AC_PATH_XTRA puts CPPFLAGS in a CFLAGS variable. Adjust here.
AM_CPPFLAGS = $(X_CFLAGS) -D_NO_DXm
if RPATHLINK
AM_LDFLAGS = $(RPATHLINK)=@MAKESHLIBDIR@
endif
LDADD = -L@MAKESHLIBDIR@ $(X_PRE_LIBS) $(X_LIBS) $(MOTIF_LIBS) $(X_EXTRA_LIBS)

#MOTIF_LIBS = @MOTIF_LDSHARE@ @LIBXP@ @LIBXEXT@
MOTIF_LIBS = -lMrm -lXm -lXt -lX11

# Static data installed to lib
datadir = $(libdir)
data_DATA = dwscope_setup.ps

# Scripts that are copied directly
bin_SCRIPTS = ScopePrinters dwscopePrint

# dw programs
bin_PROGRAMS = dwscope
dwscope_SOURCES = dwscope.c dwscopesubs.c evaluate.c
dwscope_CPPFLAGS = $(AM_CPPFLAGS) -D_LOCAL_ACCESS
dwscope_LDADD = $(LDADD) -lMdsShr -lTdiShr -lTreeShr -lXmdsShr

bin_PROGRAMS += dwscope_remote
dwscope_remote_SOURCES = $(dwscope_SOURCES) RemoteDummies.c
dwscope_remote_LDADD = $(LDADD) -lMdsIpShr @MAKESHLIBDIR@/libXmdsShr.a -lm

bin_PROGRAMS += dwpad
dwpad_SOURCES = dwpad.c dwscopesubs.c
dwpad_LDADD = $(LDADD) -lXmdsShr

# User interface descriptions, installed as generic data but with build rules
uiddir = @uiddir@
uid_DATA = dwscope.uid dwpad.uid
%.uid: %.uil
	@UIL@ -I$(top_srcdir)/include -o $@ $<

MOSTLYCLEANFILES = $(uid_DATA)
