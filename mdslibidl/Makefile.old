include $(MAKE_OPTIONS)

OBJECTS = MdsLibIdl.o

EXPORTS = mdslibidl.export

mdslibidl : ../shlib ../shlib/libMdsLibIdl.$(SHARE_TYPE)

../shlib/libMdsLibIdl.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) $(IDL_STATIC) -L../shlib -lTdiShr -lTreeShr -lMdsShr

../shlib :
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libMdsLibIdl.$(SHARE_TYPE)
	- rm .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
