include $(MAKE_OPTIONS)

OBJECTS = \
	mdsipshr.o

mdstcpip : ../bin ../shlib ../bin/mdsiptest

../bin :
	mkdir ../bin

../bin/mdsiptest : t.c ../shlib/libMdsIpShr.$(SHARE_TYPE)
	cc -o $@ $(CFLAGS) t.c -L../shlib -lMdsIpShr $(SOCKET_LIB)

../shlib/libMdsIpShr.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -lc $(SOCKET_LIB)

../shlib :
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libMdsIpShr.$(SHARE_TYPE)
	- rm .depend
	- rm ../bin/mdsiptest

.depend :
	- rm .depend
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

