include $(MAKE_OPTIONS)

OBJECTS = \
        mdsptdata.o

mdslib : ../shlib ../shlib/libMdsD3D.$(SHARE_TYPE)  

../shlib :
	mkdir ../shlib

mdsptdata.o : mdsptdata.f
	f77 -c  -I. $(D3DFFLAGS) -o $@ mdsptdata.f

../shlib/libMdsD3D.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS)  $(D3LIBOPTIONS)

clean :
	- rm *.o
	- rm ../shlib/libMdsD3D.$(SHARE_TYPE)
	- rm .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)


