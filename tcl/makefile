include $(MAKE_OPTIONS)
LFLAGS = -L$(MDSPLUS)/shlib

OBJFILES = 			\
	mds_dsc_string.o	\
	tcl_dummy.o		\
	tcl_add_node.o	\
	tcl_add_tag.o		\
	tcl_clean_datafile.o		\
	tcl_close.o		\
	tcl_compress_datafile.o		\
	tcl_create_pulse.o	\
	tcl_decompile.o	\
	tcl_delete_node.o	\
	tcl_delete_pulse.o	\
	tcl_directory.o	\
	tcl_directory_tag.o	\
	tcl_dispatch.o		\
	tcl_do_method.o	\
	tcl_do_node.o	\
	tcl_edit.o		\
	tcl_put_expression.o		\
	tcl_remove_tag.o	\
	tcl_rename.o		\
	tcl_set_callbacks.o	\
	tcl_set_current.o	\
	tcl_set_default.o	\
	tcl_set_node.o	\
	tcl_set_tree.o	\
	tcl_show_current.o	\
	tcl_show_data.o	\
	tcl_show_db.o	\
	tcl_verify_tree.o	\
	tcl_write.o

EXPORTS = tcl_commands.export

all:	../bin/tcl tcl_commands.o  libTcl.a	\
			../shlib/tcl_commands.$(SHARE_TYPE)

clean:
	rm ../bin/tcl
	find . -name libTcl.a -exec rm {} \;
	find ../shlib -name tcl_commands.$(SHARE_TYPE) -exec rm {} \;
	rm tcl_commands.o
	rm tcl_commands.c
	rm .depend

../bin/tcl:	tcl
	chmod +x tcl
	cp  tcl ../bin

tcl_commands.c:	tcl_commands.ocld
	../bin/cdu < tcl_commands.ocld

tcl_commands.o:	tcl_commands.c

libTcl.a:	$(OBJFILES)
	$(AR) -r libTcl.a $(OBJFILES)
	$(RANLIB)

../shlib/tcl_commands.$(SHARE_TYPE):	libTcl.a 
	$(LD)  -o $@ $(LFLAGS)	\
		$(LD_SHARED_LIB_OPTIONS)	\
		tcl_commands.o  libTcl.a	\
		-lMdsdcl -lMdsShr -lTreeShr -lTdiShr -lMdsServerShr -lc

.depend :
	- find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)

