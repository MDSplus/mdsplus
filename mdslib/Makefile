include $(MAKE_OPTIONS)

OBJECTS = \
        MdsLib.o 

mdslibjeff : ../bin ../shlib ../shlib/libMdsLib.$(SHARE_TYPE)

../bin :
	mkdir ../bin

../shlib/libMdsLib.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -lc $(SOCKET_LIB) -L /f/mdsplus/cvsroot/mdsplus/shlib -lMdsIpShr -lTdiShr -lTreeShr -lMdsShr

../shlib :
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libMdsLib.$(SHARE_TYPE)
	- rm .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

