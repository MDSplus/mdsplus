include $(MAKE_OPTIONS)
LFLAGS = -L$(MDSPLUS)/shlib

OBJFILES = 		\
mdsdcl_dcl_parse.o		\
mdsdcl_do_command.o	\
mdsdcl_dummy.o		\
mdsdcl_general.o		\
mdsdcl_get_input.o		\
mdsdcl_initialize.o	\
mdsdcl_insert_symbols.o	\
mdsdcl_macros.o		\
mdsdcl_set_command.o	\
mdsdcl_spawn.o		\
mdsdcl_wait.o

EXPORTS = mdsdclshr.export

all:	libMdsdcl.a  ../bin/mdsdcl  ../shlib/libMdsdcl.$(SHARE_TYPE)

clean:
	find ../shlib -name libMdsdcl.$(SHARE_TYPE) -exec rm {} \;
	find ../bin  -name mdsdcl  -exec rm {} \;
	rm  *.a  *.o .depend
	rm mdsdcl_tables.c

mdsdcl_tables.o:	mdsdcl_tables.c

mdsdcl_tables.c:	mdsdcl_tables.ocld
	../bin/cdu < mdsdcl_tables.ocld

libMdsdcl.a:	$(OBJFILES)
		$(AR) -r libMdsdcl.a $(OBJFILES)
		$(RANLIB)

../shlib/libMdsdcl.$(SHARE_TYPE):	mdsdcl_tables.o  libMdsdcl.a \
					../cdu/libCdu.a
	$(LD)  -o $@ $(LFLAGS)	\
		$(LD_SHARED_LIB_OPTIONS)	\
		mdsdcl_tables.o		\
		$(EXPORT_SYMBOLS) libMdsdcl.a	\
		$(EXPORT_SYMBOLS)  $(MDSPLUS)/cdu/libCdu.a	\
		-lMdsShr  -lc -lm

../bin/mdsdcl:	mdsdcl.o  ../shlib/libMdsdcl.$(SHARE_TYPE)
	$(CC) $(CFLAGS) -o $@ $(LFLAGS) mdsdcl.o	\
		-lMdsdcl -lMdsShr

.depend:
	- find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
	
