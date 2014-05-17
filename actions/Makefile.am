# Global variables

#AC_PATH_XTRA puts CPPFLAGS in a CFLAGS variable. Adjust here.
AM_CPPFLAGS = $(X_CFLAGS)
if RPATHLINK
AM_LDFLAGS = $(RPATHLINK)=$(libdir)
endif
LDADD = -L@MAKESHLIBDIR@ -lXmdsShr -lTreeShr -lTdiShr -lMdsShr $(X_PRE_LIBS) $(X_LIBS) $(MOTIF_LIBS) $(X_EXTRA_LIBS)

MOTIF_LIBS = -lMrm -lXm -lXt -lX11

#FIXME: Remove this when LIBS is cleaned up in configure
LIBS=

bin_PROGRAMS = actions actmon

actions_SOURCES = actions.c

actmon_SOURCES = actmon.c
actmon_LDADD = $(LDADD) -lMdsIpShr -lMdsServerShr

# User interface descriptions, installed as generic data but with build rules
uiddir = @MAKEUIDDIR@
uid_DATA = actions.uid actmon.uid
%.uid: %.uil
	@UIL@ -I$(top_srcdir)/include -o $@ $<

