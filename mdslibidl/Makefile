include $(MAKE_OPTIONS)

OBJECTS = MdsLibIdl.o

all :	../shlib/libMdsLibIdl$(SHARE_TYPE) \
	../idl \
	../idl/mdsopen.pro \
	../idl/mdsclose.pro \
	../idl/mdssetdefault.pro \
	../idl/mdsvalue.pro \
	../idl/mdsput.pro \
	../idl/mdsidlimage.pro

../shlib/libMdsLibIdl$(SHARE_TYPE) : $(OBJECTS)
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -L../shlib -lMdsShr -lTdiShr -lTreeShr -lc -lm

../idl :
	mkdir ../idl

../idl/mdsopen.pro : mdsopen.pro
	cp mdsopen.pro ../idl/mdsopen.pro

../idl/mdsclose.pro : mdsclose.pro
	cp mdsclose.pro ../idl/mdsclose.pro

../idl/mdssetdefault.pro : mdssetdefault.pro
	cp mdssetdefault.pro ../idl/mdssetdefault.pro

../idl/mdsvalue.pro : mdsvalue.pro
	cp mdsvalue.pro ../idl/mdsvalue.pro

../idl/mdsput.pro : mdsput.pro
	cp mdsput.pro ../idl/mdsput.pro

../idl/mdsidlimage.pro : mdsidlimage.pro
	cp mdsidlimage.pro ../idl/mdsidlimage.pro

clean :
	- rm *.o
	- rm ../shlib/libMdsLibIdl$(SHARE_TYPE)
	- rm .depend

depend : .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include .depend
