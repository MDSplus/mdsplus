include $(MAKE_OPTIONS)
LFLAGS = -L$(MDSPLUS)/shlib

OBJFILES = 			\
	libTcl.a(mds_dsc_string.o)	\
	libTcl.a(tcl_dummy.o)		\
	libTcl.a(tcl_add_node.o)	\
	libTcl.a(tcl_add_tag.o)		\
	libTcl.a(tcl_close.o)		\
	libTcl.a(tcl_create_pulse.o)	\
	libTcl.a(tcl_decompile.o)	\
	libTcl.a(tcl_directory.o)	\
	libTcl.a(tcl_directory_tag.o)	\
	libTcl.a(tcl_do_method.o)	\
	libTcl.a(tcl_set_callbacks.o)	\
	libTcl.a(tcl_set_current.o)	\
	libTcl.a(tcl_set_default.o)	\
	libTcl.a(tcl_set_node.o)	\
	libTcl.a(tcl_set_tree.o)	\
	libTcl.a(tcl_show_data.o)	\
	libTcl.a(tcl_verify_tree.o)	\
	libTcl.a(tcl_write.o)

all:	tcl tcl_commands.o  libTcl.a	\
			../shlib/tcl_commands.$(SHARE_TYPE)

clean:
	find . -name libTcl.a -exec rm {} \;

tcl:
	chmod +x tcl

tcl_commands.c:	tcl_commands.ocld
	../cdu/cdu < tcl_commands.ocld

tcl_commands.o:	tcl_commands.c

libTcl.a:	$(OBJFILES)
	ranlib libTcl.a

../shlib/tcl_commands.$(SHARE_TYPE):	tcl_commands.o  libTcl.a
	$(CC) $(CFLAGS)  -o $@ $(LFLAGS) $(LD_SHARED_LIB_OPTIONS)	\
		tcl_commands.o  libTcl.a	\
		-lMdsdcl -lMdsShr -lTreeShr -lTdiShr	\
		$(MDSPLUS)/cdu/libCdu.a

.depend:
	touch .depend
