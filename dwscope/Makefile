include $(MAKE_OPTIONS)

CFLAGS = $(MOTIF_CFLAGS)

OBJECTS_DWSCOPE = \
dwscope.o \
dwscopesubs.o \
evaluate.o

OBJECTS_DWPAD = \
dwpad.o \
dwscopesubs.o

'dwscope' : ../bin \
../bin/dwscope \
../uid/dwscope.uid \
../bin/dwpad \
../uid/dwpad.uid

../bin :
	mkdir ../bin

../bin/dwscope : $(OBJECTS_DWSCOPE)
	cc -o $@ $(OBJECTS_DWSCOPE)  -L../shlib -lMdsShr -lTreeShr -lTdiShr -lXmdsShr $(MOTIF_LIBS) $(X11_LIBS)

../uid/dwscope.uid : dwscope.uil
	uil -o $@ dwscope.uil

../bin/dwpad : $(OBJECTS_DWPAD)
	cc -o $@ $(OBJECTS_DWPAD) -L../shlib -lXmdsShr $(MOTIF_LIBS) $(X11_LIBS)

../uid/dwpad.uid : dwpad.uil
	uil -o $@ dwpad.uil
 
clean :
	- rm .depend
	- rm *.o
	- rm ../bin/dwscope
	- rm ../uid/dwscope.uid
	- rm ../bin/dwpad
	- rm ../uid/dwpad.uid

.depend :
	- rm .depend
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
