include $(MAKE_OPTIONS)

OBJECTS = \
	main-uil.o \
	bxutils-uil.o \
	CallbacksUil.o \
        ReadInt.o \
        ReadString.o \
        GetNciString.o \
        GetSupportedDevices.o

CFLAGS = $(MOTIF_CFLAGS)

all : ../bin/traverser \
	../uid/traverser.uid

../bin/traverser : $(OBJECTS) ../bin
	 cc -o $@ $(OBJECTS)  $(CC_IMAGE_OPTIONS) -L../shlib -lMdsShr -lTreeShr -lTdiShr -lXmdsShr $(MOTIF_LIBS) $(X11_LIBS)

../bin :
	mkdir ../bin

../uid/traverser.uid : traverser.uil
	uil -wmd ../shlib/xmdsuil.wmd -o $@ traverser.uil
	
clean :
	- rm *.o
	- rm ../bin/traverser
	- rm ../uid/traverser.uid
	- rm .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

