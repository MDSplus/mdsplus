#!/bin/sh
# macmake.sh
# October 3, 2002 - Darren Garnier
# this is a makefile for testing the build on MacOS X.  
# It will probably go away after the port is complete.
#
unset CLASSPATH
#PATH=/sbin:/usr/sbin:/bin:/usr/bin:/usr/local/bin:/usr/X11R6/bin:/sw/bin ; export PATH
#DYLD_LIBRARY_PATH==/usr/local/lib:/usr/X11R6/lib; export DYLD_LIBRARY_PATH
#LD_LIBRARY_PATH=/usr/local/lib:/usr/X11R6/lib; export LD_LIBRARY_PATH
#C_INCLUDE_PATH=/usr/local/include ; export C_INCLUDE_PATH
#DYLD_FALLBACK_LIBRARY_PATH=`pwd`/lib ; export DYLD_FALLBACK_LIBRARY_PATH
#LD_FALLBACK_LIBRARY_PATH=`pwd`/lib ; export LD_FALLBACK_LIBRARY_PATH
MACOSX_DEPLOYMENT_TARGET=10.3 ; export MACOSX_DEPLOYMENT_TARGET
#
# mitdevices needs tree to be working for TdiCompile to function properly
#
MDSPLUS_DIR=`pwd` ; export MDSPLUS_DIR
MDS_PATH="$MDSPLUS_DIR/tdi" ; export MDS_PATH
UIDPATH="$MDSPLUS_DIR/uid/%U" ; export UIDPATH
MDS_LIB_PS="$MDSPLUS_DIR/lib/dwscope_setup.ps" ; export MDS_LIB_PS
main_path=`pwd`/trees ; export main_path
subtree_path=`pwd`/trees/subtree ; export subtree_path
export JDK_DIR=/Library/Java/Home
# this is to use the Sybase Open Client Libraries
export SYBASE=/usr/local/sybase
# to use freetds for database
# defined by taking freetds and doing a 
# configure --prefix=/usr/local/freetds --enable-sybase-compat
# make ; sudo make install
# currently freetds bombs on connection from MDSplus... I don't know why.
#export SYBASE=/usr/local/freetds

#
printenv
if (test configure.in -nt configure) then 
    autoconf 
    rm config.log config.status
fi
if (test configure -nt Makefile ) then
    ./configure exec_prefix=/usr/local/mdsplus 
fi
make
cp lib/*.dylib /usr/local/mdsplus/lib
