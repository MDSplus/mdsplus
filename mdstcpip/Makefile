include $(MAKE_OPTIONS)

OBJECTS = \
	mdsipshr.o

OBJECTS_UTIL = \
	mdsiputil.o

EXPORTS = mdsipshr.export

mdstcpip : ../bin ../shlib ../bin/mdsiptest ../bin/mdsip ../bin/mdsipd

../bin :
	mkdir ../bin

../bin/mdsiptest : t.c ../shlib/libMdsIpShr.$(SHARE_TYPE)
	$(CC) -o $@ $(CFLAGS) t.c -L../shlib -lMdsIpShr $(SOCKET_LIB)

../bin/mdsip : mdsip_u.c
	$(CC) -o $@ $(CFLAGS) mdsip_u.c -L../shlib -lMdsIpShr -lMdsShr -lTreeShr -lTdiShr $(SOCKET_LIB)

../bin/mdsipd : mdsipd
	cp mdsipd ../bin/mdsipd

../shlib/libMdsIpShr.$(SHARE_TYPE) : $(OBJECTS) libMdsIpUtil.a
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) libMdsIpUtil.a -lc $(SOCKET_LIB) 

../shlib :
	mkdir ../shlib

libMdsIpUtil.a :  $(OBJECTS_UTIL)
	$(AR) -r $@ $(OBJECTS_UTIL)
	

clean :
	- rm *.o
	- rm ../shlib/libMdsIpShr.$(SHARE_TYPE)
	- rm .depend
	- rm ../bin/mdsiptest
	- rm ../bin/mdsip

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

