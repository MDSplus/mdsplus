include $(MAKE_OPTIONS)
LFLAGS = -L$(MDSPLUS)/shlib

all:	../shlib/ccl_commands.$(SHARE_TYPE)  ccl

ccl_commands.o:	ccl_commands.c
	$(CC) $(CFLAGS) -c ccl_commands.c

ccl_commands.c:	ccl_commands.ocld
	../cdu/cdu < ccl_commands.ocld

../shlib/ccl_commands.$(SHARE_TYPE):	ccl_commands.o  ccl_dummy.o
	$(CC) $(CFLAGS) -o $@ $(LD_SHARED_LIB_OPTIONS)	\
			ccl_commands.o ccl_dummy.o  -lc

ccl:
	chmod +x ccl

.depend:
	touch .depend
