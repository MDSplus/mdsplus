include $(MAKE_OPTIONS)

OBJECTS = \
	dummies.o \
	librtl.o \
	MdsCmprs.o \
	MdsCompareXd.o \
	MdsCompress.o \
	MdsGet1DxA.o \
	MdsGet1DxS.o \
        MdsGetSetShotId.o \
	MdsXdRoutines.o \
	MdsPk.o

../shlib/libMdsShr.$(SHARE_TYPE) : $(OBJECTS) ../shlib
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -lc

../shlib :
	mkdir ../shlib

clean :
	if (test -f *.o) then rm *.o; fi
	if (test -f ../shlib/libMdsShr.$(SHARE_TYPE)) then rm ../shlib/libMdsShr.$(SHARE_TYPE); fi
	if (test -f .depend) then rm .depend; fi

depend : .depend

.depend :
	- find *.c -exec $(MAKE_DEPEND)

include .depend
