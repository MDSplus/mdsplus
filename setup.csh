# This procedure defines environment variables for csh and tcsh shells
# You must cd to the mdsplus directory and then source this file:
# for example:
# 
# cd ~/mdsplus
# source setup.csh
#
set os=`uname`
set cwd=`pwd`
if ( ! $?IDL_PATH ) setenv IDL_PATH " "
eval `awk '{ if ($1 == "source") print $0 ";" ; else if ($1 == ".") print "" ; else if ($1 !~ /^#.*/) print "setenv " $1 " " $2 ";"}' envsyms sys_specific/envsyms.$os`
