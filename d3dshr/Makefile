include $(MAKE_OPTIONS)

D3DLIB = /c/osf/lib

OBJECTS = \
        mdsptdata.o

mdslib : ../shlib ../shlib/libMdsD3D.$(SHARE_TYPE)  

../shlib :
	mkdir ../shlib

mdsptdata.o : mdsptdata.f
	f77 -c -g -I. -o $@ mdsptdata.f

../shlib/libMdsD3D.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS)  -L$(D3DLIB) -ld3 -lc -lfor

clean :
	- rm *.o
	- rm ../shlib/libMdsD3D.$(SHARE_TYPE)
	- rm .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

