include $(MAKE_OPTIONS)

OBJECTS = \
	mdsipshr.o vmshpwd.o

OBJECTS_UTIL = \
	mdsiputil.o

EXPORTS = mdsipshr.export

mdstcpip : ../bin ../shlib ../etc ../bin/mdsiptest ../bin/mdsip ../bin/mdsipd ../etc/mdsip.hosts ../shlib/libMdsIpShr.a

../bin :
	mkdir ../bin

../bin/mdsiptest : t.c ../shlib/libMdsIpShr.$(SHARE_TYPE)
	$(CC) -o $@ $(CFLAGS) t.c -L../shlib -lMdsIpShr $(SOCKET_LIB)

../bin/mdsip : mdsip_u.c
	$(CC) -o $@ $(CFLAGS) mdsip_u.c -L../shlib -lMdsIpShr -lMdsShr -lTreeShr -lTdiShr $(SOCKET_LIB)

../bin/mdsipd : mdsipd
	cp mdsipd ../bin/mdsipd

../shlib/libMdsIpShr.$(SHARE_TYPE) : $(OBJECTS) libMdsIpUtil.a
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) $(OBJECTS_UTIL) -lc $(SOCKET_LIB) 

../shlib/libMdsIpShr.a : $(OBJECTS) libMdsIpUtil.a
	$(AR) -r $@ $(OBJECTS) ../mdstcpip/libMdsIpUtil.a 

../shlib :
	mkdir ../shlib

../etc :
	mkdir ../etc

../etc/mdsip.hosts : mdsip.hosts
	cp mdsip.hosts ../etc/mdsip.hosts

libMdsIpUtil.a :  $(OBJECTS_UTIL)
	$(AR) -r $@ $(OBJECTS_UTIL)

clean :
	- rm *.o
	- rm ../shlib/libMdsIpShr.$(SHARE_TYPE)
	- rm ../shlib/libMdsIpShr.a
	- rm .depend
	- rm ../bin/mdsiptest
	- rm ../bin/mdsip
	- rm ../etc/mdsip.hosts

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

