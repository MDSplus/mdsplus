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
if [ -z "$EXPORTCMD" ]
then
  awkexportcmd="export \" \$1 \" ;\""
else
  awkexportcmd="export \" \$1 \" ; $EXPORTCMD \" \$1 \" \$\" \$1 \" ;\""
fi

doExport()
{
  eval export $1="\$$1"
  if ( test "$EXPORTCMD" != "" )
  then
    $EXPORTCMD $1 "$(printenv $1)"
  fi
}

if [ -n "$MDSPLUS_DIR" ]; then
  :
elif [ -r /etc/.mdsplus_dir ]; then
  MDSPLUS_DIR=`/bin/cat /etc/.mdsplus_dir`
else
  MDSPLUS_DIR=/usr/local/mdsplus
fi
doExport MDSPLUS_DIR

if ( test "$temp_sym_name" != "")
then
  eval temp_sym_old_value=$`echo $temp_sym_name`
  if ( test "$temp_sym_old_value" = "" )
  then
    eval $temp_sym_name='`echo $temp_sym_value`'
    doExport $temp_sym_name
  else
    found=false
    for v in `echo ${temp_sym_old_value} | tr "${temp_delim}" "\n"`
    do
        if ( test "$v" = "$temp_sym_value")
        then
            found=true
            break
        fi
    done
    if ( test "$found" = "true" ) ; then
        :
    else
      case $temp_direction in
      '>')
        eval $temp_sym_name='`echo ${temp_sym_old_value}${temp_delim}${temp_sym_value}`'
        doExport $temp_sym_name;;
      '<')
        eval $temp_sym_name='`echo ${temp_sym_value}${temp_delim}${temp_sym_old_value}`'
        doExport $temp_sym_name;;
      *)
        echo bad direction - $temp_direction;;
      esac
    fi
  fi
  unset temp_sym_name
  unset temp_sym_old_value
  unset temp_direction
  unset temp_delim
else
  if ( test "$temp_file" = "")
  then
    if ( test "$setup_file" != "" )
    then
      temp_file=$setup_file
    else
      if ( test -r ./envsyms )
      then
        temp_file=envsyms
      else
        if ( test -r $MDSPLUS_DIR/etc/envsyms )
        then
          temp_file=$MDSPLUS_DIR/etc/envsyms
        else
          temp_file=asdjklasdjasdjlkasd
        fi
      fi
    fi
  fi
  if ( test -r $temp_file )
  then
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
    awkcmd="$awkcmd"'{ if ((NF > 0) && ($1 !~ /^#.*/) && (NF < 3) && ($1 != "source") && ($1 != ".") && ($1 != "include")) print $1 "=" $2 "; '$awkexportcmd' ;'
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
  fi
fi
if [ -z "$PyLib" ]
then
  pyver="$(python -V 2>&1)"
  if [ $? = 0 -a "$pyver" != "" ]
  then
    PyLib=$(echo $pyver | awk '{print $2}' 2>/dev/null | awk -F. '{print "python"$1"."$2}' 2>/dev/null)
    if [ $? = 0 ]
    then
      doExport PyLib
    fi
  fi
fi
