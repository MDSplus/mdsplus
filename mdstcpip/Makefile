include $(MAKE_OPTIONS)

OBJECTS = \
	mdsipshr.o

mdstcpip : ../bin ../shlib ../bin/mdsiptest ../bin/mdsip

../bin :
	mkdir ../bin

../bin/mdsiptest : t.c ../shlib/libMdsIpShr.$(SHARE_TYPE)
	cc -o $@ $(CFLAGS) t.c -L../shlib -lMdsIpShr $(SOCKET_LIB)

../bin/mdsip : mdsip_u.c
	cc -o $@ $(CFLAGS) mdsip_u.c -L../shlib -lMdsIpShr -lMdsShr -lTreeShr -lTdiShr $(SOCKET_LIB)

../shlib/libMdsIpShr.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -lc $(SOCKET_LIB)

../shlib :
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libMdsIpShr.$(SHARE_TYPE)
	- rm .depend
	- rm ../bin/mdsiptest
	- rm ../bin/mdsip

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

