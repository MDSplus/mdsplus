include $(MAKE_OPTIONS)
LFLAGS = -L$(MDSPLUS)/shlib

all:	../shlib/ccl_commands.$(SHARE_TYPE)  ../bin/ccl

clean:
	rm *.a *.o ../shlib/ccl_commands.$(SHARE_TYPE)

ccl_commands.o:	ccl_commands.c

ccl_commands.c:	ccl_commands.ocld
	../cdu/cdu < ccl_commands.ocld

../shlib/ccl_commands.$(SHARE_TYPE):	ccl_commands.o  ccl_dummy.o
	$(LD)  -o $@  $(LD_SHARED_LIB_OPTIONS)	\
		ccl_commands.o ccl_dummy.o  -lc

../bin/ccl:	ccl
	chmod +x ccl
	cp ccl ../bin

.depend:
	touch .depend
