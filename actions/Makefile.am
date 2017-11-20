# Global variables

#AC_PATH_XTRA puts CPPFLAGS in a CFLAGS variable. Adjust here.
AM_CPPFLAGS = $(TARGET_ARCH) $(WARNFLAGS) $(X_CFLAGS)
if RPATHLINK
AM_LDFLAGS = $(RPATHLINK)=@MAKESHLIBDIR@ $(TARGET_ARCH)
endif

actlog_SOURCES = actlog.c
actlog_LDADD = -L@MAKESHLIBDIR@ -lTreeShr -lTdiShr -lMdsShr -lMdsIpShr -lMdsServerShr

uiddir = @uiddir@

if HAVE_MOTIF


LDADD = -L@MAKESHLIBDIR@ -lXmdsShr -lTreeShr -lTdiShr -lMdsShr $(X_PRE_LIBS) $(X_LIBS) $(MOTIF_LIBS) $(X_EXTRA_LIBS)

bin_PROGRAMS = actions actmon actlog

MOTIF_LIBS = -lMrm -lXm -lXt -lX11

actmon_SOURCES = actmon.c
actmon_LDADD = $(LDADD) -lMdsIpShr -lMdsServerShr

actions_SOURCES = actions.c

# User interface descriptions, installed as generic data but with build rules
uid_DATA = actions.uid actmon.uid
%.uid: %.uil
	@UIL@ -I$(top_srcdir)/include -o $@ $<

MOSTLYCLEANFILES = $(uid_DATA)


else


bin_PROGRAMS = actlog


endif
