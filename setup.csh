set os=`uname`
eval `awk '$1 !~ /#/ {print "setenv " $1 " " $2 ";"}' envsyms sys_specific/envsyms.$os`
