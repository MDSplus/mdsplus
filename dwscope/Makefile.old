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
	$(CC) -o $@ $(OBJECTS_DWSCOPE)  evaluate_local.o -L../shlib -lXmdsShr $(MOTIF_LIBS)  $(X11_LIBS) -lTdiShr -lTreeShr -lMdsShr $(DLD) -lm

../bin/dwscope_remote : $(OBJECTS_DWSCOPE) evaluate.o RemoteDummies.o
	$(CC) -o $@ $(OBJECTS_DWSCOPE)  evaluate.o RemoteDummies.o \
                  -L../shlib $(STATIC_LIB) -lXmdsShr $(SHARED_LIB) -lMdsIpShr $(MOTIF_LIBS) $(X11_LIBS) -lm $(DLD)

../uid/dwscope.uid : dwscope.uil
	uil -o $@ dwscope.uil

../bin/dwpad : $(OBJECTS_DWPAD)
	$(CC) -o $@ $(OBJECTS_DWPAD) -L../shlib -lXmdsShr $(MOTIF_LIBS) $(X11_LIBS) -lTdiShr -lTreeShr -lMdsShr $(DLD) -lm

../uid/dwpad.uid : dwpad.uil
	uil -o $@ dwpad.uil
 
evaluate_local.o : evaluate.c
	$(CC) -c $(CFLAGS) -D_LOCAL_ACCESS -o $@ evaluate.c

clean :
	- rm .depend
	- rm *.o
	- rm ../bin/dwscope
	- rm ../bin/dwscope_remote
	- rm ../uid/dwscope.uid
	- rm ../bin/dwpad
	- rm ../uid/dwpad.uid

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
