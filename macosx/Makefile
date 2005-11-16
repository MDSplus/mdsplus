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
export SYBASE=/usr/local/sybase
# to use freetds for database
# defined by taking freetds and doing a 
# configure --prefix=/usr/local/freetds --enable-sybase-compat
# make ; sudo make install
# currently freetds bombs on connection from MDSplus... I don't know why.
#export SYBASE=/usr/local/freetds

export MACOSX_DEPLOYMENT_TARGET=10.3

all: $(src_dir)/Makefile
	$(MAKE) -C $(src_dir)

$(src_dir)/configure: $(src_dir)/configure.in
	cd $(src_dir) && autoconf

$(src_dir)/Makefile: $(src_dir)/configure $(src_dir)/configure.in $(src_dir)/Makefile.in
	cd $(src_dir) && ./configure --prefix=/usr/local/mdsplus --exec_prefix=/usr/local/mdsplus

install: $(src_dir)/Makefile
	$(MAKE) -C $(src_dir) install

binaries:  
	cd $(src_dir)/macosx/installer && make all
 
.IGNORE:

clean:
	$(MAKE) -C $(src_dir) clean
	cd $(src_dir)/macosx/installer && make clean

