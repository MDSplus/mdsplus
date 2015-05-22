# Global variables

#AC_PATH_XTRA puts CPPFLAGS in a CFLAGS variable. Adjust here.
AM_CPPFLAGS = $(X_CFLAGS) $(TARGET_ARCH)
if RPATHLINK
AM_LDFLAGS = $(RPATHLINK)=@MAKESHLIBDIR@ $(TARGET_ARCH)
endif
LDADD = -L@MAKESHLIBDIR@ -lXmdsShr -lTreeShr -lTdiShr -lMdsShr $(LIBS) $(X_PRE_LIBS) $(X_LIBS) $(MOTIF_LIBS) $(X_EXTRA_LIBS)

MOTIF_LIBS = -lMrm -lXm -lXt -lX11

#FIXME: Remove this when LIBS is cleaned up in configure
#LIBS=

bin_PROGRAMS = actions actmon

actions_SOURCES = actions.c

actmon_SOURCES = actmon.c
actmon_LDADD = $(LDADD) -lMdsIpShr -lMdsServerShr

# User interface descriptions, installed as generic data but with build rules
uiddir = @uiddir@
uid_DATA = actions.uid actmon.uid
%.uid: %.uil
	@UIL@ -I$(top_srcdir)/include -o $@ $<

