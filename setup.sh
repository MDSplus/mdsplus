os=`uname`
cwd=`pwd`
eval `awk '$1 !~ /#/ {print $1 "=" $2 "; export " $1 ";"}' envsyms sys_specific/envsyms.$os`
