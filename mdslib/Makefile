include $(MAKE_OPTIONS)

OBJECTS = \
        MdsLib.o 

mdslib : ../bin ../shlib ../shlib/libMdsLib.$(SHARE_TYPE) ../shlib/libMdsLib.a mdslib_ctest  mdslib_ftest  mdslib_fremotetest

../bin :
	mkdir ../bin

../shlib/libMdsLib.$(SHARE_TYPE) : $(OBJECTS) ../mdstcpip/libMdsIpUtil.a
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) ../mdstcpip/libMdsIpUtil.a -lc $(SOCKET_LIB) -L../shlib -lTdiShr -lTreeShr -lMdsShr 

../shlib/libMdsLib.a : $(OBJECTS) ../mdstcpip/libMdsIpUtil.a
	$(AR) -r $@ $(OBJECTS) ../mdstcpip/libMdsIpUtil.a 

../shlib :
	mkdir ../shlib

mdslib_ctest : mdslib_ctest.c ../shlib/libMdsLib.$(SHARE_TYPE)
	cc -o $@ $(CFLAGS) mdslib_ctest.c -L../shlib -lMdsLib

mdslib_ftest : mdslib_ftest.f ../shlib/libMdsLib.$(SHARE_TYPE)
	f77 -o $@ $(FFLAGS) mdslib_ftest.f -lMdsLib -lTreeShr

mdslib_fremotetest : mdslib_fremotetest.f ../shlib/libMdsLib.$(SHARE_TYPE)
	f77 -o $@ $(FFLAGS) mdslib_fremotetest.f -lMdsLib

clean :
	- rm *.o
	- rm ../shlib/libMdsLib.$(SHARE_TYPE)
	- rm mdslib_?test
	- rm .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

