include $(MAKE_OPTIONS)

OBJECTS = \
        MdsLib.o 

mdslib : ../bin ../shlib ../shlib/libMdsLib.$(SHARE_TYPE)  mdslib_ctest  mdslib_ftest

../bin :
	mkdir ../bin

../shlib/libMdsLib.$(SHARE_TYPE) : $(OBJECTS) ../mdstcpip/libMdsIpUtil.a
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) ../mdstcpip/libMdsIpUtil.a -lc $(SOCKET_LIB) -L../shlib -lTdiShr -lTreeShr -lMdsShr 

../shlib :
	mkdir ../shlib

mdslib_ctest : mdslib_ctest.o ../shlib/libMdsLib.$(SHARE_TYPE)
	cc -o $@ $(CFLAGS) mdslib_ctest.c -I../include -L../shlib -lMdsLib

mdslib_ftest : mdslib_ftest.f ../shlib/libMdsLib.$(SHARE_TYPE)
	f77 -o $@ mdslib_ftest.f -g -V +es +U -v -I../include -Wl,-L../shlib -lMdsLib

clean :
	- rm *.o
	- rm ../shlib/libMdsLib.$(SHARE_TYPE)
	- rm mdslib_?test
	- rm .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

