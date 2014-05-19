libdir=@MAKESHLIBDIR@
AM_LDFLAGS = -L$(libdir) -lMdsShr

if RPATHLINK
RPATHFLAGS = $(RPATHLINK)=$(libdir)
else
RPATHFLAGS =
endif

bin_PROGRAMS = setevent
setevent_SOURCES = setevent.c
setevent_LDFLAGS = $(AM_LDFLAGS) $(RPATHFLAGS)

