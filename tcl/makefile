include $(MAKE_OPTIONS)
LFLAGS = -L$(MDSPLUS)/shlib

OBJFILES = 			\
	libTcl.a(mds_dsc_string.o)	\
	libTcl.a(tcl_dummy.o)		\
	libTcl.a(tcl_add_node.o)	\
	libTcl.a(tcl_add_tag.o)		\
	libTcl.a(tcl_clean_datafile.o)		\
	libTcl.a(tcl_close.o)		\
	libTcl.a(tcl_compress_datafile.o)		\
	libTcl.a(tcl_create_pulse.o)	\
	libTcl.a(tcl_decompile.o)	\
	libTcl.a(tcl_delete_node.o)	\
	libTcl.a(tcl_delete_pulse.o)	\
	libTcl.a(tcl_directory.o)	\
	libTcl.a(tcl_directory_tag.o)	\
	libTcl.a(tcl_dispatch.o)		\
	libTcl.a(tcl_do_method.o)	\
	libTcl.a(tcl_do_node.o)	\
	libTcl.a(tcl_edit.o)		\
	libTcl.a(tcl_put_expression.o)		\
	libTcl.a(tcl_remove_tag.o)	\
	libTcl.a(tcl_rename.o)		\
	libTcl.a(tcl_set_callbacks.o)	\
	libTcl.a(tcl_set_current.o)	\
	libTcl.a(tcl_set_default.o)	\
	libTcl.a(tcl_set_node.o)	\
	libTcl.a(tcl_set_tree.o)	\
	libTcl.a(tcl_show_current.o)	\
	libTcl.a(tcl_show_data.o)	\
	libTcl.a(tcl_show_db.o)	\
	libTcl.a(tcl_verify_tree.o)	\
	libTcl.a(tcl_write.o)

EXPORTS = tcl_commands.export

all:	../bin/tcl tcl_commands.o  libTcl.a	\
			../shlib/tcl_commands.$(SHARE_TYPE)

clean:
	find . -name libTcl.a -exec rm {} \;
	find ../shlib -name tcl_commands.$(SHARE_TYPE) -exec rm {} \;
	rm tcl_commands.c

../bin/tcl:	tcl
	chmod +x tcl
	cp  tcl ../bin

tcl_commands.c:	tcl_commands.ocld
	../bin/cdu < tcl_commands.ocld

tcl_commands.o:	tcl_commands.c

libTcl.a:	$(OBJFILES)
	$(RANLIB)

../shlib/tcl_commands.$(SHARE_TYPE):	libTcl.a
	$(LD)  -o $@ $(LFLAGS)	\
		$(LD_SHARED_LIB_OPTIONS)	\
		tcl_commands.o  libTcl.a	\
		-lMdsdcl -lMdsShr -lTreeShr -lTdiShr -lc

.depend:
	touch .depend
