os=`uname`
cwd=`pwd`
eval `awk '{ if ($1 == "source") print "" ; else if ($1 == ".") print $0 ";"; else if ($1 !~ /^#.*/) print $1 "=" $2 "; export " $1 ";"}' envsyms`
