include $(MAKE_OPTIONS)

OBJECTS = \
	actions.o

CFLAGS = $(MOTIF_CFLAGS)

all : ../bin/actions \
	../uid/actions.uid

../bin/actions : $(OBJECTS) ../bin
	 $(CC) -o $@ $(OBJECTS)  $(CC_IMAGE_OPTIONS) -L../shlib -lXmdsShr $(MOTIF_LIBS) $(X11_LIBS) -lTdiShr -lTreeShr -lMdsShr $(DLD) -lm

../bin :
	mkdir ../bin

../uid/actions.uid : actions.uil
	uil -o $@ actions.uil

clean :
	- rm *.o
	- rm ../bin/actions
	- rm ../uid/actions.uid
	- rm .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

