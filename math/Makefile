include $(MAKE_OPTIONS)

OBJECTS = \
	caxpy.o \
	cgeco.o \
	cmul.o  \
	csint.o  \
	csval.o  \
	daxpy.o  \
	dcsint.o  \
	dcsval.o  \
	dgeco.o  \
	dmul.o  \
	dpermu.o  \
	dppval.o  \
	dsrch.o  \
	dsvrgn.o  \
	gn.o  \
	isrch.o  \
	permu.o  \
	ppval.o  \
	saxpy.o  \
	sgeco.o  \
	smul.o  \
	srch.o  \
	ssrch.o  \
	svign.o  \
	svrgn.o  \
	umach.o  \
	zaxpy.o  \
	zgeco.o  \
	zmul.o  

math : ../shlib ../shlib/libMdsMath.$(SHARE_TYPE) 

../shlib/libMdsMath.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) 

../shlib :
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libMdsMath.$(SHARE_TYPE)
	- rm .depend



