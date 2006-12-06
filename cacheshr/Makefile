SHELL=/bin/sh
CC=gcc
LD=ld
top_srcdir=..
RM=$(top_srcdir)/devscripts/rm_if
prefix=/usr/local
exec_prefix=${prefix}
CFLAGS=-g -O0   -I$(top_srcdir)/include -fpic -fno-strict-aliasing 
CXXFLAGS=-g -O0   -I$(top_srcdir)/include -fpic -fno-strict-aliasing 
LDFLAGS= -g
LIBS=-L$(top_srcdir)/lib/ -lMdsShr -lpthread -lc  
INSTALL=../conf/install-sh -c

SOURCES = \
CachedTreeshr.c 

CXXSOURCES = \
Cache.cpp \
Notifier.cpp \
SharedMemManager.cpp \
TreeWriter.cpp \
FreeSpaceManager.cpp \
SharedDataManager.cpp \
SharedMemTree.cpp\
Thread.cpp 

OBJECTS = $(SOURCES:.c=.o) $(CXXSOURCES:.cpp=.o)
EXPORTS = CacheShr.exports

all : $(top_srcdir)/lib/ $(top_srcdir)/lib/ $(top_srcdir)/lib/libCacheShr.so $(top_srcdir)/lib/libCacheShr.a

clean:
	@ $(RM) $(OBJECTS)
	@ $(RM) so_locations
	@ $(RM) $(top_srcdir)/lib/libCacheShr.so
	@ $(RM) $(top_srcdir)/lib/libCacheShr.a

depend:
	 @makedepend -- $(CFLAGS) -- $(SOURCES)

install:
	if [ ! -d ${exec_prefix}/lib ] ; then \
                mkdir ${exec_prefix}/lib; \
                chmod 755 ${exec_prefix}/lib; \
        fi;
	$(INSTALL) -m 755  $(top_srcdir)/lib/libTreeShr.so ${exec_prefix}/lib
	if test .so != .a ; then \
		$(INSTALL) -m 644  $(top_srcdir)/lib/libTreeShr.a ${exec_prefix}/lib; \
	fi;


$(top_srcdir)/lib/libCacheShr.so : $(OBJECTS)  
	$(LD) -g -o $@ -shared  $(OBJECTS) $(LIBS)

$(top_srcdir)/lib/libCacheShr.a : $(OBJECTS)
	$(AR) -cr $@ $(OBJECTS)
	ranlib $@

$(top_srcdir)/lib/ :
	mkdir $(top_srcdir)/lib/


Cache.o: Cache.cpp Cache.h

NotifyServer.o: NotifyServer.cpp NotifyServer.h


SharedDataManager.o: SharedMemManager.cpp SharedDataManager.h LockManager.h CallbackManager.h Notifier.h


TreeWriter.o: TreeWriter.cpp TreeWriter.h


FreeSpaceManager.o: FreeSpaceManager.cpp FreeSpaceManager.h


SharedDataManager.o: SharedDataManager.cpp SharedDataManager.h LockManager.h


SharedMemTree.o: SharedMemTree.cpp SharedMemTree.h


Thread.o: Thread.cpp Thread.h

Notifier.o: Notifier.cpp Notifier.h
 
