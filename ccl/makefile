include $(MAKE_OPTIONS)
LFLAGS = -L$(MDSPLUS)/shlib

all:	../shlib/ccl_commands.$(SHARE_TYPE)  ccl

../shlib/ccl_commands.$(SHARE_TYPE):	ccl_commands.o  ccl_dummy.o
	$(CC) $(CFLAGS) -o $@ $(LD_SHARED_LIB_OPTIONS)	\
			ccl_commands.o ccl_dummy.o  -lc

ccl:	ccl.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LFLAGS) -lMdsdcl

.depend:
	touch .depend
