include $(MAKE_OPTIONS)

OBJECTS = \
	dummies.o  \
	librtl.o \
	dasutil_routines4.o \
	mdsmsg.o \
	MdsCmprs.o \
	MdsCompareXd.o \
	MdsCompress.o \
	MdsEventAst.o \
	MdsGet1DxA.o \
	MdsGet1DxS.o \
        MdsGetSetShotId.o \
        MdsPk.o \
        MdsQ_Event.o \
        MdsSerialize.o \
        MdsWtEvent.o \
	MdsXdRoutines.o

EXPORTS = MdsShr.export

mdsshr : ../shlib ../shlib/libMdsShr.$(SHARE_TYPE) ../shlib/libMdsShr.a

../shlib/libMdsShr.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) 

../shlib/libMdsShr.a : $(OBJECTS)
	$(AR) -r $@ $(OBJECTS)

../shlib :
	mkdir ../shlib

clean :
	rm *.o
	rm ../shlib/libMdsShr.*
	rm .depend

.depend :
	- find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
