include $(MAKE_OPTIONS)

OBJECTS = \
	mdsipshr.o

t : t.c ../shlib/libMdsIpShr.$(SHARE_TYPE)
	cc -o $@ $(CFLAGS) t.c -L../shlib -lMdsIpShr 

../shlib/libMdsIpShr.$(SHARE_TYPE) : $(OBJECTS) ../shlib
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -lc

../shlib :
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libMdsIpShr.$(SHARE_TYPE)
	- rm .depend
	- rm t

depend : .depend

.depend :
	- rm .depend
	find *.c -exec $(MAKE_DEPEND)

include .depend

