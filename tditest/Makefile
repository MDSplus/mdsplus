include $(MAKE_OPTIONS)

OBJECTS = \
tditest.o

_tditest : ../bin/tditest ../bin

../bin/tditest : $(OBJECTS)
	cc -o $@ $(OBJECTS) $(CC_IMAGE_OPTIONS) -lc -L../shlib -lTdiShr 

../bin :
	mkdir ../bin

clean :
	- rm .depend
	- rm *.o
	- rm ../bin/tditest

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
