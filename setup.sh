if (test "$setup_file" = "") then
setup_file="envsyms"
fi
awkcmd='{ if (($3 != "") && ((substr($3,2,1) == ":") || (substr($3,2,1) == ";"))) $3 = substr($3,1,1) "\\" substr($3,2) } '
awkcmd="$awkcmd"'{ if ($1 == "source") print "" ; '
awkcmd="$awkcmd"'else if ($1 == ".") print $0 ";"; '
awkcmd="$awkcmd"'else if (($1 !~ /^#.*/) && (("'$setup_operation'" == "unset"))) print "unset " $1 ";" ; '
awkcmd="$awkcmd"'else if (($1 !~ /^#.*/) && ((ENVIRON[$1] == "") || (NF < 3))) print $1 "=" $2 "; export " $1 ";" ; '
awkcmd="$awkcmd"'else if (($1 !~ /^#.*/) && (substr($3,1,1) == "<")) print $1 "=" $2 substr($3,2) "$" $1 "; export " $1 ";" ;'
awkcmd="$awkcmd"'else if (($1 !~ /^#.*/) && (substr($3,1,1) == ">")) print $1 "=$" $1 substr($3,2) $2 "; export " $1 ";" }'
eval `awk "$awkcmd" $setup_file`
