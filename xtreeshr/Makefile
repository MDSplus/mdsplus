SHELL=/bin/sh
CC=gcc
LD=ld
prefix=/usr/local
top_srcdir=..
exec_prefix=${prefix}
RM=$(top_srcdir)/devscripts/rm_if
CFLAGS=-g -O2   -I$(top_srcdir)/include -fpic -fno-strict-aliasing 
LDFLAGS=
LIBS=-lpthread -lc  -ldl -lCacheShr
INSTALL=../conf/install-sh -c

SOURCES = \
        XTreeConvertToLongTime.c\
	XTreeDefaultSquish.c \
	XTreeDefaultResample.c \
	XTreeGetTimedRecord.c

OBJECTS = $(SOURCES:.c=.o)
EXPORTS = XTreeShr.exports

all : $(top_srcdir)/lib/ $(top_srcdir)/lib/ $(top_srcdir)/lib/libXTreeShr.so $(top_srcdir)/lib/libXTreeShr.a

depend:
	@makedepend -- $(CFLAGS) -- $(SOURCES)

clean :
	@ $(RM) $(OBJECTS)
	@ $(RM) so_locations
	@ $(RM) $(top_srcdir)/lib/libXTreeShr.a 
	@ $(RM) $(top_srcdir)/lib/libXTreeShr.so 

install:
	if [ ! -d ${exec_prefix}/lib ] ; then \
                mkdirhier ${exec_prefix}/lib; \
                chmod 755 ${exec_prefix}/lib; \
        fi;
	$(INSTALL) -m 755  $(top_srcdir)/lib/libXTreeShr.so ${exec_prefix}/lib
	if (test .so != .a) then \
		$(INSTALL) -m 644  $(top_srcdir)/lib/libXTreeShr.a ${exec_prefix}/lib; \
	fi;

$(top_srcdir)/lib/libXTreeShr.so : $(OBJECTS) 
	$(LD) -o $@ -shared   -L$(top_srcdir)/lib/ $(OBJECTS) $(LIBS)  -lMdsShr -lTreeShr -lTdiShr -lCacheShr

$(top_srcdir)/lib/libXTreeShr.a : $(OBJECTS)
	$(AR) -cr $@ $(OBJECTS)
	ranlib $@

$(top_srcdir)/lib/ :
	mkdir $(top_srcdir)/lib/



XTreeShr.exports: XTreeShr.def
	sed -e /LIBRARY/\;/\;/d XTreeShr.def > $@
