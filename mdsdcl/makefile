include $(MAKE_OPTIONS)
LFLAGS = -L$(MDSPLUS)/shlib

OBJFILES = 		\
mdsdcl_dcl_parse.o	\
mdsdcl_do_command.o	\
mdsdcl_dummy.o		\
mdsmsg.o		\
mdsdcl_general.o	\
mdsdcl_get_input.o	\
mdsdcl_initialize.o	\
mdsdcl_insert_symbols.o	\
mdsdcl_macros.o		\
mdsdcl_set_command.o	\
mdsdcl_spawn.o		\
mdsdcl_wait.o

all:	mdsdcl_tables.o  libMdsdcl.a	\
	../shlib/libMdsdcl.$(SHARE_TYPE)  mdsdcl

mdsdcl_tables.o:

libMdsdcl.a:	libMdsdcl.a( $(OBJFILES) )
		ranlib libMdsdcl.a

../shlib/libMdsdcl.$(SHARE_TYPE):	libMdsdcl.a
	$(CC) $(CFLAGS)  -o $@ $(LD_SHARED_LIB_OPTIONS) -lc	\
		mdsdcl_tables.o  libMdsdcl.a  $(MDSPLUS)/cdu/libCdu.a

mdsdcl:	mdsdcl.o  ../shlib/libMdsdcl.$(SHARE_TYPE)
	$(CC) $(CFLAGS) -o $@ $(LFLAGS) $@.o -lMdsdcl		\
		$(MDSPLUS)/cdu/libCdu.a

.depend:
	touch .depend
