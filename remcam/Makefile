include $(MAKE_OPTIONS)

OBJECTS = \
	CamIosb.o \
        CamMulti.o \
	CamSingle.o

remcam : ../shlib ../shlib/libRemCamShr.$(SHARE_TYPE)

../shlib/libRemCamShr.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -lc -L../shlib -lMdsIpShr

../shlib :
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libRemCamShr.$(SHARE_TYPE)
	- rm .depend

.depend :
	- find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
