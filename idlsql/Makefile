# Generated automatically from Makefile.in by configure.
SHELL=/bin/sh
CC=cc
LD=ld
top_srcdir=..
prefix=/usr/local
exec_prefix=${prefix}
CFLAGS=-g -I$(top_srcdir)/include -pthread -I/usr/local/sybase/include -I/usr/local/rsi/idl/external 
LDFLAGS= -L/usr/local/rsi/idl/bin/bin.alpha -lidl
LIBS=-L/usr/local/sybase/lib -no_so  -lsybdb -so_archive  -L/usr/local/rsi/idl/bin/bin.alpha -lidl -lc  -ldnet_stub -lc 
INSTALL=../conf/install-sh -c

SOURCES = \
dynamic_syb.c \
idlsql_syb.c \
user_syb.c

OBJECTS = $(SOURCES:.c=.o)
EXPORTS = IdlSql.export

all : $(top_srcdir)/lib/ $(top_srcdir)/lib/libIdlSql.so $(top_srcdir)/lib/libIdlSql.a

clean:
	- rm *.o
	- rm $(top_srcdir)/lib/libIdlSql.so
	- rm $(top_srcdir)/lib/libIdlSql.a

depend:
	 @makedepend -- $(CFLAGS) -- $(SOURCES)

install:
	if [ ! -d ${exec_prefix}/lib ] ; then \
                mkdir ${exec_prefix}/lib; \
                chmod 755 ${exec_prefix}/lib; \
        fi;
	$(INSTALL) -m 644  $(top_srcdir)/lib/libIdlSql.a ${exec_prefix}/lib
	$(INSTALL) -m 755  $(top_srcdir)/lib/libIdlSql.so ${exec_prefix}/lib

$(top_srcdir)/lib/libIdlSql.so : $(OBJECTS) 
	$(LD) -o $@ -shared -rpath $(exec_prefix)/lib $(OBJECTS) -L$(top_srcdir)/lib/ $(LDFLAGS) $(LIBS)

$(top_srcdir)/lib/libIdlSql.a : $(OBJECTS)
	$(AR) -cr $@ $(OBJECTS)
	ranlib $@

$(top_srcdir)/lib/ :
	mkdir $(top_srcdir)/lib/
# DO NOT DELETE

dynamic_syb.o: /usr/include/stdlib.h /usr/include/standards.h
dynamic_syb.o: /usr/include/getopt.h /usr/include/sys/types.h
dynamic_syb.o: /usr/include/mach/machine/vm_types.h /usr/include/sys/select.h
dynamic_syb.o: /usr/include/string.h /usr/include/strings.h
dynamic_syb.o: /usr/include/stdio.h /usr/include/sys/seek.h
dynamic_syb.o: /usr/include/va_list.h /usr/include/sys/limits.h
dynamic_syb.o: /usr/include/sys/machine/machlimits.h
dynamic_syb.o: /usr/include/sys/syslimits.h
dynamic_syb.o: /usr/include/sys/machine/machtime.h
dynamic_syb.o: /usr/include/sys/rt_limits.h
dynamic_syb.o: /usr/local/sybase/include/sybfront.h
dynamic_syb.o: /usr/local/sybase/include/cstypes.h
dynamic_syb.o: /usr/local/sybase/include/csconfig.h
dynamic_syb.o: /usr/local/sybase/include/sybdb.h
dynamic_syb.o: /usr/local/sybase/include/syblogin.h
idlsql_syb.o: /usr/include/stdio.h /usr/include/standards.h
idlsql_syb.o: /usr/include/sys/seek.h /usr/include/va_list.h
idlsql_syb.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
idlsql_syb.o: /usr/include/sys/select.h /usr/include/getopt.h
idlsql_syb.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
idlsql_syb.o: /usr/include/sys/syslimits.h
idlsql_syb.o: /usr/include/sys/machine/machtime.h
idlsql_syb.o: /usr/include/sys/rt_limits.h
idlsql_syb.o: /usr/local/rsi/idl/external/export.h /usr/include/string.h
idlsql_syb.o: /usr/include/strings.h /usr/include/signal.h
idlsql_syb.o: /usr/include/sys/siginfo.h /usr/include/sys/sysmisc.h
idlsql_syb.o: /usr/include/machine/signal.h /usr/include/machine/context.h
idlsql_syb.o: /usr/include/sys/procset.h /usr/local/sybase/include/sybfront.h
idlsql_syb.o: /usr/local/sybase/include/cstypes.h
idlsql_syb.o: /usr/local/sybase/include/csconfig.h
idlsql_syb.o: /usr/local/sybase/include/sybdb.h
idlsql_syb.o: /usr/local/sybase/include/syblogin.h
user_syb.o: /usr/include/stdio.h /usr/include/standards.h
user_syb.o: /usr/include/sys/seek.h /usr/include/va_list.h
user_syb.o: /usr/include/sys/types.h /usr/include/mach/machine/vm_types.h
user_syb.o: /usr/include/sys/select.h /usr/include/getopt.h
user_syb.o: /usr/include/sys/limits.h /usr/include/sys/machine/machlimits.h
user_syb.o: /usr/include/sys/syslimits.h /usr/include/sys/machine/machtime.h
user_syb.o: /usr/include/sys/rt_limits.h /usr/include/string.h
user_syb.o: /usr/include/strings.h /usr/local/sybase/include/sybfront.h
user_syb.o: /usr/local/sybase/include/cstypes.h
user_syb.o: /usr/local/sybase/include/csconfig.h
user_syb.o: /usr/local/sybase/include/sybdb.h
user_syb.o: /usr/local/sybase/include/syblogin.h
