include $(MAKE_OPTIONS)

OBJECTS = \
        MdsLib.o 

mdslib : ../bin ../shlib ../shlib/libMdsLib.$(SHARE_TYPE)

../bin :
	mkdir ../bin

../shlib/libMdsLib.$(SHARE_TYPE) : $(OBJECTS) ../mdstcpip/libMdsIpUtil.a
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) ../mdstcpip/libMdsIpUtil.a -lc $(SOCKET_LIB) -L../shlib -lTdiShr -lTreeShr -lMdsShr 

../shlib :
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libMdsLib.$(SHARE_TYPE)
	- rm .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

