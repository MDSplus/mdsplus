export MDSPLUS=`pwd`
#
# TDI Functions
#
export MDS_PATH=$MDSPLUS/tdi
#
# Test trees
#
export main_path=$MDSPLUS/trees
export subtree_path=$main_path/subtree
#
# UID path
#
export UIDPATH=$MDSPLUS/uid/%U
#
# IDL path
#
export IDL_PATH=$MDSPLUS/idl
#
# Make options file
#
export MAKE_OPTIONS=$MDSPLUS/MakeOptions.`uname`
export DEPEND=.depend
#
# Platform specific setup
#
. setup.`uname`.ksh
