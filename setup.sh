if (test "$setup_file" = "") then
setup_file="envsyms"
fi
awkcmd='{ if (($3 != "") && ((substr($3,2,1) == ":") || (substr($3,2,1) == ";"))) $3 = substr($3,1,1) "\\" substr($3,2) } '
awkcmd="$awkcmd"'{ if ($1 == "source") print "" ; '
awkcmd="$awkcmd"'else if ($1 == ".") print $0 ";"; '
awkcmd="$awkcmd"'else if (($1 !~ /^#.*/) && (("'$setup_operation'" == "unset"))) print "unset " $1 ";" ; '
awkcmd="$awkcmd"'else if (($1 !~ /^#.*/) &&  (NF < 3)) print $1 "=" $2 "; export " $1 ";" ; '
awkcmd="$awkcmd"'else if (($1 !~ /^#.*/) && (substr($3,1,1) == "<")) print "if (test \"$" $1 "\" = \"\") then " $1 "=" $2 "; " '
awkcmd="$awkcmd"'" else " $1 "=" $2 substr($3,2) "$" $1 "; fi; export " $1 ";"  ;'
awkcmd="$awkcmd"'else if (($1 !~ /^#.*/) && (substr($3,1,1) == ">")) print "if (test \"$" $1 "\" = \"\") then " $1 "=" $2 "; " '
awkcmd="$awkcmd"'" else " $1 "=$" $1 substr($3,2) $2 "; fi; export " $1 ";" }'
eval `/bin/awk "$awkcmd" $setup_file`
