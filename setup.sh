# mdsplus.sh  - to be run during login from sh or derivative
# 
# this script will search for the first environment variable definition file
# it can find looking in the following order:
#               $setup_file
#               ./envsyms
#               $MDSPLUS_DIR/etc/envsyms
#
#
# The definition file should contain the following types of lines:
#
#   1) comment-line
#   2) variable-definition
#   3) include-statement
#   4) source-statement
#   5) dot-statement
#
# where:
#
#   1) comment-line:
#
#      A comment line beginning with # in first character.
#
#   2) variable-definition:
#
#      A environment variable definition of the following form:
#
#         environment-variable-name  environment-variable-value optional-delimiter-spec
#
#         where:
#
#            optional-delimiter-spec is the character ">" or "<" followed by a delimiter
#
#         Examples:
#
#            myvar foo                 (define environment variable myvar to have the value foo)
#            mytree_path $HOME/mytree  <; (define environment variable mytree_path to have the
#                                          value $HOME/mytree. If mytree_path was previously
#                                          defined, mytree_path will become $HOME/mytree followed
#                                          by a semi-colon followed by the original value.)
#            PATH $HOME/bin  :>           (Append $HOME/bin to the PATH environment variable.)
#
#   3) include-statement:
#
#        Include another definition file.
#
#        Example:
#
#            include $HOME/.mdsplus
#
#   4) source-statement
#
#        Noop in sh userd when same definition file is sourced from mdsplus.csh
#
#   5) dot-statement
#
#        Source another file with shell commands
# 
#        Example:
#
#            . $HOME/myscript.csh
#
#
set_symbol()
{
# $1 - symbol name
# $2 - symbol value
# $3 - append (>) or prepend (<) or set ()
# $4 - delimiter
  temp_sym_old_value=${!1}
  if [ -z "$temp_sym_old_value" ]
  then
    eval $1=\"${2}\"
  else
    if [ $2 = \"\" ]
    then
      do_it=1
    else
      echo $temp_sym_old_value | /bin/grep `eval echo $2` > /dev/null
      do_it=$?
    fi
    if [ $do_it != 0 ]
    then
      case $3 in
      '>')
        eval $1=\"$temp_sym_old_value\"\"$4\"\"$2\";;
      '<')
        eval $1=\"$2\"\"$4\"\"$temp_sym_old_value\";;
      '')
	eval $1=\"${2}\";;
      *)
        echo bad direction - set_symbol $1 $2 $3 $4;;
      esac
    fi
  fi
  unset temp_sym_old_value do_it
}

include ()
{
  if [ -r $1 ]
  then
    if [ ! -z "$verify_setup" ]; then echo Processing $1; fi
    eval `/bin/awk -f - $1 <<EOF
{
  if ((NF > 0) && (\\$1 !~ /^#.*/) && (NF < 3) && (\\$1 != "source") && (\\$1 != ".") && (\\$1 != "include")) 
    print "set_symbol \\'" \\$1 "\\' \\'" \\$2 "\\'; export \\'" \\$1 "\\';";
  else if ((NF > 0) && (\\$1 == "source")) print "" ;
  else if ((NF > 0) && (\\$1 == ".")) print \\$0 ";" ;
  else if ((NF > 0) && (\\$1 == "include")) print \\$0 ";";
  else if ((NF > 0) && (\\$1 !~ /^#.*/)) print "set_symbol \\'" \\$1 "\\' \\'" \\$2 "\\' \\'" substr(\\$3,1,1) "\\' \\'" substr(\\$3,2) "\\'; export \\'" \\$1 "\\';"
}
EOF
`
  fi
}

if [ -z "$MDSPLUS_DIR" ]
then
  MDSPLUS_DIR=/usr/local/mdsplus
  if [ -r /etc/.mdsplus_dir ]
  then
<<<<<<< setup.sh
    MDSPLUS_DIR=`/bin/cat /etc/.mdsplus_dir`
=======
    if ( test -r ./setup.sh )
    then
      temp_setup_script=./setup.sh
    else 
      if ( test -r $MDSPLUS_DIR/setup.sh )
      then
        temp_setup_script=$MDSPLUS_DIR/setup.sh
      else
        temp_setup_script=unknown_setup_script
      fi
    fi
    if ( test "$verify_setup" != "" )
    then
      echo Processing $temp_file
    fi
    awkcmd='{ if (($3 != "") && ((substr($3,2,1) == ":") || (substr($3,2,1) == ";"))) $3 = substr($3,1,1) "\\" substr($3,2) } '
    awkcmd="$awkcmd"'{ if ((NF > 0) && ($1 !~ /^#.*/) && (NF < 3) && ($1 != "source") && ($1 != ".") && ($1 != "include")) print $1 "=" $2 "; export " $1 " ;" ;'
    awkcmd="$awkcmd"'else if ((NF > 0) && ($1 == "source")) print "" ; '
    awkcmd="$awkcmd"'else if ((NF > 0) && ($1 == "include")) print "temp_file=" $2 "; . '$temp_setup_script' ; " ; '
    awkcmd="$awkcmd"'else if ((NF > 0) && ($1 == ".")) print $0 ";" ; '
    awkcmd="$awkcmd"'else if ((NF > 0) && ($1 !~ /^#.*/)) print "temp_sym_name=" $1 "; temp_sym_value=" $2 "; temp_direction='
    awkcmd="$awkcmd""'"
    awkcmd="$awkcmd"'" substr($3,1,1) "'
    awkcmd="$awkcmd""'"
    awkcmd="$awkcmd"'; temp_delim='
    awkcmd="$awkcmd"'" substr($3,2) "'
    awkcmd="$awkcmd"'; . '$temp_setup_script';" ; }'
    if ( test -x /bin/awk ) 
    then
      shellcmd=`/bin/awk "$awkcmd" $temp_file`
    else
      if ( test -x /usr/bin/awk) 
      then
        shellcmd=`/usr/bin/awk "$awkcmd" $temp_file`
      else
        shellcmd=`awk "$awkcmd" $temp_file`
      fi
    fi
    unset temp_file
    eval $shellcmd
#    unset temp_setup_script
    unset shellcmd
    unset awkcmd
  else
    unset temp_file
>>>>>>> 1.18
  fi
fi
export MDSPLUS_DIR
if [ -r ./envsyms ]
then
  include ./envsyms 
else
  include $MDSPLUS_DIR/etc/envsyms
fi
unset include set_symbol
