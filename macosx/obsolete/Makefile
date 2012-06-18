# macosx/Makefile
# 
# April 4, 2005 - Darren Garnier
# this is a makefile for building on MacOS X.  
# It sets a few extra environment variables and runs various other make/configure files.
unexport CLASSPATH

#
# mitdevices needs tree to be working for TdiCompile to function properly
#
export src_dir=..
export MDSPLUS_DIR=$(src_dir)

export MDS_PATH=$(MDSPLUS_DIR)/tdi 
export UIDPATH=$(MDSPLUS_DIR)/uid/%U 
export MDS_LIB_PS=$(MDSPLUS_DIR)/lib/dwscope_setup.ps 
export main_path=$(MDSPLUS_DIR)/trees
export subtree_path=$(MDSPLUS_DIR)/trees/subtree 
export JDK_DIR=/Library/Java/Home
# this is to use the Sybase Open Client Libraries
#export SYBASE=/usr/local/sybase
# to use freetds for database
# defined by taking freetds and doing a 
# configure --prefix=/usr/local/freetds --enable-sybase-compat
# make ; sudo make install
# currently freetds bombs on connection from MDSplus... I don't know why.
export SYBASE=/usr/local/freetds

ECHO=echo

export MACOSX_DEPLOYMENT_TARGET=10.4

all: $(src_dir)/Makefile
	$(MAKE) -C $(src_dir)

$(src_dir)/configure: $(src_dir)/configure.in
	cd $(src_dir) && autoconf

$(src_dir)/Makefile: $(src_dir)/configure $(src_dir)/configure.in $(src_dir)/Makefile.in Makefile
	cd $(src_dir) && \
	./configure --prefix=/usr/local/mdsplus --exec_prefix=/usr/local/mdsplus

universal: $(src_dir)/configure $(src_dir)/configure.in $(src_dir)/Makefile.in Makefile
	cd $(src_dir) && \
	env \
		./configure --prefix=/usr/local/mdsplus --exec_prefix=/usr/local/mdsplus \
			--enable-universal
#		F77=fort77 \
#		CFLAGS="-O2 -g -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc" \
#		LDFLAGS="-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk,-arch,i386,-arch,ppc" \
#		FFLAGS="-O2 -g -Wc,-isysroot,/Developer/SDKs/MacOSX10.4u.sdk,-arch,i386,-arch,ppc" \
#		FLIBS=" " \
#		FLIBS="-Wc,-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk,-arch,i386,-arch,ppc" \
	$(ECHO) "#include <machine/endian.h>" 			>> ../include/config.h
	$(ECHO) "#undef WORDS_BIGENDIAN" 			>> ../include/config.h
	$(ECHO) "#undef _big_endian"	 			>> ../include/config.h
	$(ECHO) "#if BYTE_ORDER==BIG_ENDIAN" 			>> ../include/config.h
	$(ECHO) "# define WORDS_BIGENDIAN 1" 			>> ../include/config.h
	$(ECHO) "# define _big_endian 1"	 		>> ../include/config.h
	$(ECHO) "#endif"					>> ../include/config.h
	sed -e 's/CFLAGS=/CFLAGS=-I\/usr\/local\/include /' \
		-e 's/svign/svign_f2c/'   \
		-e 's/svrgn/svrgn_f2c/'   \
		-e 's/umach/umach_f2c/'   \
		../math/Makefile > ../math/Makefile.tmp
	mv ../math/Makefile.tmp ../math/Makefile

install: $(src_dir)/Makefile
	$(MAKE) -C $(src_dir) install

binaries:  
	cd $(src_dir)/macosx/installer && make all
 
.IGNORE:

clean:
	$(MAKE) -C $(src_dir) clean
	cd $(src_dir)/macosx/installer && make clean

