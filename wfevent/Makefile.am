bindir = @MAKEBINDIR@
bin_PROGRAMS = wfevent

libdir=@MAKESHLIBDIR@

wfevent_SOURCES = wfevent.c
wfevent_LDADD = -L$(libdir) -lTdiShr -lTreeShr -lMdsShr
#FIXME: Use rpath-link until build system is flattened
if RPATHLINK
wfevent_LDFLAGS = -Wl,-rpath-link=$(libdir)
endif

#FIXME: Clean up LIBS in top level configure
LIBS=

