AM_CFLAGS = $(TARGET_ARCH) $(WARNFLAGS)

#FIXME: Use rpath-link until build system is flattened
libdir=@MAKESHLIBDIR@
if RPATHLINK
RPATHFLAGS = $(RPATHLINK)=$(libdir)
else
RPATHFLAGS =
endif

AM_LDFLAGS = $(RPATHFLAGS)
LDADD = -L$(libdir) -lMdsShr

bin_PROGRAMS = setevent
setevent_SOURCES = setevent.c

#FIXME: Clean up LIBS in top level configure
#LIBS=

