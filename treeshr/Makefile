include $(MAKE_OPTIONS)

OBJECTS = \
dummies.o \
TreeAddNode.o \
TreeCreatePulseFile.o \
TreeDeletePulseFile.o \
TreeDoMethod.o \
TreeFindNode.o \
TreeFindTagWild.o \
TreeGetDbi.o \
TreeGetNci.o \
TreeGetRecord.o \
TreeOpen.o \
TreePutRecord.o \
TreeSetDefault.o \
TreeSetNci.o \
RemoteAccess.o

treeshr : ../shlib ../shlib/libTreeShr.$(SHARE_TYPE)

../shlib/libTreeShr.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(MISALIGN_LD_FLAGS) $(OBJECTS) -lc -L../shlib -lMdsShr

../shlib :
	mkdir ../shlib

TreeGetNci.o : TreeGetNci.c
	cc $(CFLAGS) $(MISALIGN_CFLAGS) -c TreeGetNci.c

clean :
	- rm *.o
	- rm ../shlib/libTreeShr.$(SHARE_TYPE)
	- rm .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
