bindir = @MAKEBINDIR@
bin_PROGRAMS = wfevent

libdir=@MAKESHLIBDIR@

wfevent_SOURCES = wfevent.c
wfevent_LDADD = -L$(libdir) -lTdiShr -lMdsShr
#FIXME: Use rpath-link until build system is flattened
wfevent_LDFLAGS = -Wl,-rpath-link=$(libdir)

#FIXME: Clean up LIBS in top level configure
LIBS=

