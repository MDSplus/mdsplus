include $(MAKE_OPTIONS)

OBJECTS = \
	actions.o

CFLAGS = $(MOTIF_CFLAGS)

all : ../bin/actions \
	../uid/actions.uid

../bin/actions : $(OBJECTS) ../bin
	 cc -o $@ $(OBJECTS)  $(CC_IMAGE_OPTIONS) -L../shlib -lMdsShr -lTreeShr -lTdiShr -lXmdsShr -lMdsdcl $(MOTIF_LIBS) $(X11_LIBS)

../bin :
	mkdir ../bin

../uid/actions.uid : actions.uil
	uil -wmd ../shlib/xmdsuil.wmd -o $@ actions.uil

clean :
	- rm *.o
	- rm ../bin/actions
	- rm ../uid/actions.uid
	- rm .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

