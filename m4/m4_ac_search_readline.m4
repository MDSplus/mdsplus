dnl // Search macro for libreadline //
dnl 
dnl usage: 
dnl    AC_SEARCH_READLINE([RL])
dnl    AM_CONDITIONAL(RL, test x"$have_readline" = x"yes")
dnl 
dnl The --with-readline option is added in configure that scans also for the
dnl presence of READLINE_DIR environment variable.
dnl This defines the following variables to be added in compilation lines:
dnl    RL_CPPFLAGS, RL_LDFLAGS, RL_LIBS
dnl 


AC_DEFUN([AC_SEARCH_READLINE],
[
  AC_ARG_WITH([readline], 
              AC_HELP_STRING([--with-readline=READLINE_DIR], [look for the readline library in READLINE_DIR]),
              [],
              [AS_VAR_SET_IF([READLINE_DIR],
                             [AS_VAR_SET([with_readline], [$READLINE_DIR])],[])])

  if test x"$with_readline" != x"no" ; then
     if test -d "${with_readline}" ; then
        _readline_cppflags="-I${with_readline}/include"
        _readline_ldflags="-L${with_readline}/lib"
     fi

     dnl // search for all possible terminal combinations //
     for _termcap in "" "-ltermcap" "-lcurses" "-lncurses" ; do
        _combo="readline${_termcap:+ $_termcap}"
        
        LIBS_save=$LIBS
        CPPFLAGS_save=$CPPFLAGS
        LDFLAGS_save=$LDFLAGS
        
        dnl // setting test program flags //
        LIBS="-l$_combo $LIBS"
        CPPFLAGS="${_readline_cppflags} $CPPFLAGS $TARGET_ARCH"
        LDFLAGS="${_readline_ldflags} $LDFLAGS $TARGET_ARCH"

        AC_MSG_CHECKING([whether readline via "$_combo" is present and sane])
        dnl ////////////////////////////////////////////////////////////////////
        dnl // test program source  ////////////////////////////////////////////
        dnl ////////////////////////////////////////////////////////////////////
	AC_TRY_RUN(
                   AC_LANG_SOURCE([[
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>	
int main(int argc, char **argv)
{
  int fd;
  char *line;
  close(0);
  close(1);
  fd = open("/dev/null", 2);
  dup(fd);
  line = readline("giveittome> ");
  printf("got <%s>\n", line);
  return 0;
 }
////////////////////////////////////////////////////////////////////////////////
                    ]]),
                    dnl action if true:
                     [have_readline=yes],
                    dnl action if false:
                     [have_readline=no],
                    dnl action if cross compiling:
                    dnl If cross compilation is done the configure is not able to run the test
                      AC_SEARCH_LIBS([readline],[$_combo],[have_readline=yes],[have_readline=no])
                    ) dnl AC_TRY_RUN
        
        AC_MSG_RESULT([$have_readline])
        
        dnl restore LIBS 
        LIBS=$LIBS_save
        CPPFLAGS=$CPPFLAGS_save
        LDFLAGS=$LDFLAGS_save
        
        if test $have_readline = yes ; then 
          AC_SUBST($1[]_CPPFLAGS,"$_readline_cppflags")
          AC_SUBST($1[]_LDFLAGS, "$_readline_ldflags")
          AC_SUBST($1[]_LIBS,"-l$_combo")           
          break 
        fi
     done 
     dnl end for


     unset _termcap
     unset LIBS_save
     unset CPPFLAGS_save
     unset LDFLAGS_save
     unset _combo
  fi
])

