include $(MAKE_OPTIONS)

CFLAGS = $(MOTIF_CFLAGS)

OBJECTS_DWSCOPE = \
dwscope.o \
dwscopesubs.o

OBJECTS_DWPAD = \
dwpad.o \
dwscopesubs.o

'dwscope' : ../bin \
../bin/dwscope \
../uid/dwscope.uid \
../bin/dwpad \
../uid/dwpad.uid \
../bin/dwscope_remote

../bin :
	mkdir ../bin

../bin/dwscope : $(OBJECTS_DWSCOPE) evaluate_local.o
	cc -o $@ $(OBJECTS_DWSCOPE)  evaluate_local.o -L../shlib -lMdsShr -lTreeShr -lTdiShr -lXmdsShr $(MOTIF_LIBS) $(X11_LIBS)

../bin/dwscope_remote : $(OBJECTS_DWSCOPE) evaluate.o RemoteDummies.o
	cc -o $@ $(OBJECTS_DWSCOPE)  evaluate.o RemoteDummies.o \
                  -L../shlib $(STATIC_LIB) -lXmdsShr $(SHARED_LIB) -lMdsIpShr $(MOTIF_LIBS) $(X11_LIBS) -lm

../uid/dwscope.uid : dwscope.uil
	uil -o $@ dwscope.uil

../bin/dwpad : $(OBJECTS_DWPAD)
	cc -o $@ $(OBJECTS_DWPAD) -L../shlib -lMdsShr -lTreeShr -lTdiShr -lXmdsShr $(MOTIF_LIBS) $(X11_LIBS)

../uid/dwpad.uid : dwpad.uil
	uil -o $@ dwpad.uil
 
evaluate_local.o : evaluate.c
	$(CC) -c $(CFLAGS) -D_LOCAL_ACCESS -o $@ evaluate.c

clean :
	- rm .depend
	- rm *.o
	- rm ../bin/dwscope
	- rm ../uid/dwscope.uid
	- rm ../bin/dwpad
	- rm ../uid/dwpad.uid

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
