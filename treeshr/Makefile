include $(MAKE_OPTIONS)

OBJECTS = \
dummies.o \
TreeAddNode.o \
TreeFindNode.o \
TreeGetDbi.o \
TreeGetNci.o \
TreeGetRecord.o \
TreeOpen.o \
TreePutRecord.o \
TreeSetDefault.o \
TreeSetNci.o \
RemoteAccess.o

../shlib/libTreeShr.$(SHARE_TYPE) : $(OBJECTS) ../shlib
	ld -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -lc -L../shlib -lMdsShr

../shlib :
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libTreeShr.$(SHARE_TYPE)
	- rm .depend

depend : .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include .depend
