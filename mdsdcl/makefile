include $(MAKE_OPTIONS)
LFLAGS = -L$(MDSPLUS)/shlib

OBJFILES = 		\
libMdsdcl.a(mdsdcl_dcl_parse.o)		\
libMdsdcl.a(mdsdcl_do_command.o)	\
libMdsdcl.a(mdsdcl_dummy.o)		\
libMdsdcl.a(mdsdcl_general.o)		\
libMdsdcl.a(mdsdcl_get_input.o)		\
libMdsdcl.a(mdsdcl_initialize.o)	\
libMdsdcl.a(mdsdcl_insert_symbols.o)	\
libMdsdcl.a(mdsdcl_macros.o)		\
libMdsdcl.a(mdsdcl_set_command.o)	\
libMdsdcl.a(mdsdcl_spawn.o)		\
libMdsdcl.a(mdsdcl_wait.o)

all:	libMdsdcl.a  ../bin/mdsdcl  ../shlib/libMdsdcl.$(SHARE_TYPE)

clean:
	rm  *.a  *.o  ../shlib/libMdsdcl.$(SHARE_TYPE)  ../bin/mdsdcl

mdsdcl_tables.o:	mdsdcl_tables.c

mdsdcl_tables.c:	mdsdcl_tables.ocld
	../cdu/cdu < mdsdcl_tables.ocld

libMdsdcl.a:	$(OBJFILES)
		$(RANLIB)

../shlib/libMdsdcl.$(SHARE_TYPE):	mdsdcl_tables.o  libMdsdcl.a
	$(LD)  -o $@ $(LFLAGS)	\
		$(LD_SHARED_LIB_OPTIONS)	\
		mdsdcl_tables.o		\
		$(EXPORT_SYMBOLS) libMdsdcl.a	\
		$(EXPORT_SYMBOLS)  $(MDSPLUS)/cdu/libCdu.a	\
		-lMdsShr  -lcurses -lc

../bin/mdsdcl:	mdsdcl.o  ../shlib/libMdsdcl.$(SHARE_TYPE)
	$(CC) $(CFLAGS) -o $@ $(LFLAGS) mdsdcl.o	\
		-lMdsdcl -lMdsShr

.depend:
	touch .depend
