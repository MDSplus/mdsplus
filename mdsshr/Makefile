include $(MAKE_OPTIONS)

OBJECTS = \
	dummies.o \
	librtl.o \
	MdsCmprs.o \
	MdsCompareXd.o \
	MdsCompress.o \
	MdsGet1DxA.o \
	MdsGet1DxS.o \
	MdsXdRoutines.o \
	MdsPk.o

../shlib/libMdsShr.$(SHARE_TYPE) : $(OBJECTS) ../shlib
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -lc

../shlib :
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libMdsShr.$(SHARE_TYPE)
	- rm .depend

depend : .depend

.depend :
	- find *.c -exec $(MAKE_DEPEND)

include .depend
