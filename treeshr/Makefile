include $(MAKE_OPTIONS)

OBJECTS = \
dummies.o \
TreeAddNode.o \
TreeAddTag.o \
TreeCleanDatafile.o \
TreeCreatePulseFile.o \
TreeDeleteNode.o \
TreeDeletePulseFile.o \
TreeDoMethod.o \
TreeFindNode.o \
TreeFindTagWild.o \
TreeGetDbi.o \
TreeGetNci.o \
TreeGetRecord.o \
TreeOpen.o \
TreePutRecord.o \
TreeRenameNode.o \
TreeRemoveNodesTags.o\
TreeSerializeNci.o \
TreeSetDefault.o \
TreeSetNci.o \
TreeVerify.o \
RemoteAccess.o

EXPORTS = TreeShr.export

treeshr : ../shlib ../shlib/libTreeShr.$(SHARE_TYPE) ../shlib/libTreeShr.a

../shlib/libTreeShr.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -lc -L../shlib -lMdsShr

../shlib/libTreeShr.a : $(OBJECTS)
	$(AR) -r $@ $(OBJECTS)

../shlib :
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libTreeShr.$(SHARE_TYPE)
	- rm ../shlib/libTreeShr.a
	- rm .depend

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
