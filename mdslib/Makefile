include $(MAKE_OPTIONS)

OBJECTS = \
        MdsLib.o 

OBJECTS_CLIENT = MdsLib_client.o

mdslib : ../bin ../shlib \
         ../shlib/libMdsLib.$(SHARE_TYPE) \
         ../shlib/libMdsLib_client.$(SHARE_TYPE) \
         ../shlib/libMdsLib_client.a \
         ../shlib/libMdsLib.a mdslib_ctest  \
         mdslib_ftest  mdslib_fremotetest

../bin :
	mkdir ../bin

../shlib/libMdsLib.$(SHARE_TYPE) : $(OBJECTS) ../mdstcpip/libMdsIpUtil.a
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) ../mdstcpip/libMdsIpUtil.a -lc $(SOCKET_LIB)\
               -L../shlib -lTdiShr -lTreeShr -lMdsShr 

../shlib/libMdsLib.a : $(OBJECTS) ../mdstcpip/libMdsIpUtil.a
	$(AR) -r $@ $(OBJECTS) ../mdstcpip/libMdsIpUtil.a 

../shlib/libMdsLib_client.$(SHARE_TYPE) : $(OBJECTS_CLIENT) ../mdstcpip/libMdsIpUtil.a
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS_CLIENT) ../mdstcpip/libMdsIpUtil.a -lc $(SOCKET_LIB)

../shlib/libMdsLib_client.a : $(OBJECTS_CLIENT) ../mdstcpip/libMdsIpUtil.a
	$(AR) -r $@ $(OBJECTS) ../mdstcpip/libMdsIpUtil.a 

MdsLib_client.o : MdsLib.c
	$(CC) -o $@ -c $(CFLAGS) MdsLib.c -D_CLIENT_ONLY

../shlib :
	mkdir ../shlib

mdslib_ctest : mdslib_ctest.c ../shlib/libMdsLib.$(SHARE_TYPE)
	$(CC) -o $@ $(CFLAGS) mdslib_ctest.c -L../shlib -lMdsLib

mdslib_ftest : mdslib_ftest.f ../shlib/libMdsLib.$(SHARE_TYPE)
	f77 -o $@ $(FFLAGS) mdslib_ftest.f -lMdsLib -lTreeShr

mdslib_fremotetest : mdslib_fremotetest.f ../shlib/libMdsLib.$(SHARE_TYPE)
	f77 -o $@ $(FFLAGS) mdslib_fremotetest.f -lMdsLib

clean :
	- rm *.o
	- rm ../shlib/libMdsLib_client.$(SHARE_TYPE)
	- rm ../shlib/libMdsLib_client.a
	- rm ../shlib/libMdsLib.$(SHARE_TYPE)
	- rm ../shlib/libMdsLib.a
	- rm mdslib_?test
	- rm mdslib_?remotetest
	- rm .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

