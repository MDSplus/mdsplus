include $(MAKE_OPTIONS)
LFLAGS = -g -L$(MDSPLUS)/shlib

EXPORTS = ccl_commands.export

all:	../shlib/ccl_commands.$(SHARE_TYPE)  ../bin/ccl

clean:
	find ../shlib -name ccl_commands.$(SHARE_TYPE) -exec rm {} \;
	rm *.a *.o

ccl_commands.o:	ccl_commands.c

ccl_commands.c:	ccl_commands.ocld
	../bin/cdu < ccl_commands.ocld

../shlib/ccl_commands.$(SHARE_TYPE):	ccl_commands.o  ccl_dummy.o
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS)	\
			ccl_commands.o ccl_dummy.o  -lc

../bin/ccl:	ccl
	chmod +x ccl
	cp ccl ../bin

.depend:
	touch .depend
