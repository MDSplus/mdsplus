include $(MAKE_OPTIONS)
LFLAGS = -L$(MDSPLUS)/shlib

OBJFILES = 			\
	mds_dsc_string.o	\
	tcl_dummy.o		\
	tcl_add_node.o		\
	tcl_add_tag.o		\
	tcl_close.o		\
	tcl_create_pulse.o	\
	tcl_directory.o		\
	tcl_directory_tag.o	\
	tcl_do_method.o		\
	tcl_set_callbacks.o	\
	tcl_set_current.o	\
	tcl_set_default.o	\
	tcl_set_node.o		\
	tcl_set_tree.o		\
	tcl_show_data.o		\
	tcl_verify_tree.o

all:	clean  tcl tcl_commands.o  libTcl.a	\
			../shlib/tcl_commands.$(SHARE_TYPE)

clean:
	find . -name libTcl.a -exec rm {} \;

tcl:	tcl.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LFLAGS) -lMdsdcl	\
		$(MDSPLUS)/cdu/libCdu.a

tcl_commands.o:

libTcl.a:	libTcl.a( $(OBJFILES) )
		ranlib libTcl.a

../shlib/tcl_commands.$(SHARE_TYPE):	libTcl.a
	$(CC) $(CFLAGS)  -o $@ $(LFLAGS) $(LD_SHARED_LIB_OPTIONS)	\
		tcl_commands.o  libTcl.a	\
		-lMdsdcl -lMdsShr -lTreeShr -lTdiShr	\
		$(MDSPLUS)/cdu/libCdu.a

.depend:
	touch .depend
