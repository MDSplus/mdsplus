# mdsplus.csh  - to be sourced during login from csh or derivative 
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
#        Source another file with shell commands
#
#        Example:
#
#            source $HOME/myscript.csh
#
#   5) dot-statement
#
#        Noop in csh used when same definition file is invoked by mdsplus.sh
# 
#
#

if ( $?MDSPLUS_DIR ) then
  setenv MDSPLUS_DIR $MDSPLUS_DIR 
else if ( -r /etc/.mdsplus_dir ) then
  setenv MDSPLUS_DIR `cat /etc/.mdsplus_dir`
else
  setenv MDSPLUS_DIR /usr/local/mdsplus
endif

if ( $?temp_sym_name ) then
  set temp_sym_old_value=`printenv $temp_sym_name`
  if ( $temp_sym_old_value == '' ) then
    setenv $temp_sym_name $temp_sym_value
  else
    set found=false
    foreach v (`echo ${temp_sym_old_value} | tr "${temp_delim}" "\n"`)
      if ( $v == $temp_sym_value ) then
        set found=true
      endif
    end
    if ( $found == 'false' ) then
      switch ($temp_direction)
      case '>':
        setenv $temp_sym_name ${temp_sym_old_value}${temp_delim}${temp_sym_value}
        breaksw
      case '<':
        setenv $temp_sym_name ${temp_sym_value}${temp_delim}${temp_sym_old_value}
        breaksw
      default:
        echo bad direction - $temp_direction
        breaksw
      endsw
    endif
  endif
  unset temp_sym_name
  unset temp_sym_old_value
  unset temp_direction
  unset temp_delim
else
  if ( ! $?temp_file) then
    if ( $?setup_file ) then
      set temp_file=$setup_file
    else if ( -r ./envsyms ) then
      set temp_file=envsyms
    else if ( -r $MDSPLUS_DIR/etc/envsyms ) then
      set temp_file=$MDSPLUS_DIR/etc/envsyms
    else
      set temp_file=asdjklasdjasdjlkasd
    endif
  endif
  if ( -r $temp_file ) then
    if ( -r ./setup.csh ) then
      set temp_setup_script=./setup.csh
    else if ( -r $MDSPLUS_DIR/setup.csh ) then
      set temp_setup_script=$MDSPLUS_DIR/setup.csh
    else
      set temp_setup_script=unknown_setup_script
    endif
    if ( $?verify_setup ) echo Processing $temp_file
    set awkcmd='{ if (($3 != "") && ((substr($3,2,1) == ":") || (substr($3,2,1) == ";"))) $3 = substr($3,1,1) "\\" substr($3,2) } '
    set awkcmd="$awkcmd"'{ if ((NF > 0) && ($1 !~ /^#.*/) && (NF < 3) && ($1 != "source") && ($1 != ".") && ($1 != "include")) print "setenv " $1 " " $2 ";" ;'
    set awkcmd="$awkcmd"'else if ((NF > 0) && ($1 == "source")) print $0 ";" ; '
    set awkcmd="$awkcmd"'else if ((NF > 0) && ($1 == "include")) print "set temp_file=" $2 "; source $temp_setup_script ; " ; '
    set awkcmd="$awkcmd"'else if ((NF > 0) && ($1 == ".")) print "" ; '
    set awkcmd="$awkcmd"'else if ((NF > 0) && ($1 !~ /^#.*/)) print "set temp_sym_name=" $1 "; set temp_sym_value=" $2 "; set temp_direction='
    set awkcmd="$awkcmd""'"
    set awkcmd="$awkcmd"'" substr($3,1,1) "'
    set awkcmd="$awkcmd""'"
    set awkcmd="$awkcmd"'; set temp_delim='
    set awkcmd="$awkcmd"'" substr($3,2) "'
    set awkcmd="$awkcmd"'; source $temp_setup_script;" ; }'
    if (-x /bin/awk) then
       set shellcmd=`/bin/awk "$awkcmd" $temp_file`
    else if (-x /usr/bin/awk) then
       set shellcmd=`/usr/bin/awk "$awkcmd" $temp_file`
    else
       set shellcmd=`awk "$awkcmd" $temp_file`
    endif
    unset temp_file
    eval $shellcmd
#    unset temp_setup_script
    unset shellcmd
    unset awkcmd
  else
    unset temp_file
  endif
endif
if ( ! $?PyLib ) then
  set pyver=`python -V >& /dev/stdout`
  if ( $? == 0 ) then
    set PyLib=`echo $pyver | awk '{print $2}' | awk -F. '{print "python"$1"."$2;}'`
  endif
  unset pyver
endif
