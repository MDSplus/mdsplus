include $(MAKE_OPTIONS)

OBJECTS = \
tditest.o

../bin/tditest : $(OBJECTS) ../bin
	cc -o $@ $(OBJECTS) -lc -L../shlib -lTdiShr 

../bin :
	mkdir ../bin

clean :
	- rm .depend
	- rm *.o
	- rm ../bin/tditest

depend : .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include .depend
