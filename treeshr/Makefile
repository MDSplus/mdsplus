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
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(MISALIGN_LD_FLAGS) $(OBJECTS) -lc -L../shlib -lMdsShr

../shlib :
	mkdir ../shlib

TreeGetNci.o : TreeGetNci.c
	cc $(CFLAGS) $(MISALIGN_CFLAGS) -c TreeGetNci.c

clean :
	- rm *.o
	- rm ../shlib/libTreeShr.$(SHARE_TYPE)
	- rm .depend

depend : .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include .depend
