# This procedure defines environment variables for csh and tcsh shells
# You must cd to the mdsplus directory and then source this file:
# for example:
# 
# cd ~/mdsplus
# source setup.csh
#
#
if ( $setup_file != "" ) then
  set file=$setup_file
  set op=""
else
  set file="/usr/local/mdsplus/etc/envsyms"
  set op=""
  if ( $#argv > 0 ) then
    if ( $argv[1] == "unset" ) then
      set op=$argv[1]
    else
      if ($#argv > 1) then
        set op=$argv[2]
      endif
      set file=$argv[1]
    endif
  endif
endif
set os=`uname`
set cwd=`pwd`
set awkcmd='{ if (($3 != "") && ((substr($3,2,1) == ":") || (substr($3,2,1) == ";"))) $3 = substr($3,1,1) "\\" substr($3,2) } '
set awkcmd="$awkcmd"'{ if ($1 == "source") print $0 ";" ; '
set awkcmd="$awkcmd"'else if ($1 == ".") print "" ; '
set awkcmd="$awkcmd"'else if (($1 !~ /^#.*/) && (("'$op'" == "unset"))) print "unsetenv " $1 ";" ; '
set awkcmd="$awkcmd"'else if (($1 !~ /^#.*/) && (NF < 3)) print "setenv " $1 " " $2 ";" ;'
set awkcmd="$awkcmd"'else if (($1 !~ /^#.*/) && (substr($3,1,1) == "<")) print "if (! $?" $1 ") setenv " $1 " " $2 ";" '
set awkcmd="$awkcmd"'"if ($?" $1 ") setenv " $1 " " $2 substr($3,2) "$" $1 ";" ;'
set awkcmd="$awkcmd"'else if (($1 !~ /^#.*/) && (substr($3,1,1) == ">")) print "if (! $?" $1 ") setenv " $1 " " $2 ";" '
set awkcmd="$awkcmd"'"if ($?" $1 ") setenv " $1 " $" $1 substr($3,2) $2 ";" }'
eval `awk "$awkcmd" $file`

