dnl $Id$
dnl config.m4 for extension mdsplus

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(mdsplus, for mdsplus support,
[  --with-mdsplus             Include mdsplus support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(mdsplus, whether to enable mdsplus support,
dnl Make sure that the comment is aligned:
dnl [  --enable-mdsplus           Enable mdsplus support])

if test "$PHP_MDSPLUS" != "no"; then

  dnl # --with-mdsplus -> check with-path
  SEARCH_PATH="/usr/local/mdsplus"     
  SEARCH_FOR="/include/mdsdescrip.h"
  if test -r $PHP_MDSPLUS/; then # path given as parameter
    MDSPLUS_DIR=$PHP_MDSPLUS
  else # search default path list
    AC_MSG_CHECKING([for mdsplus files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        MDSPLUS_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$MDSPLUS_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the mdsplus distribution])
  fi

  dnl # --with-mdsplus -> add include path
  PHP_ADD_INCLUDE($MDSPLUS_DIR/include)

  dnl # --with-mdsplus -> check for lib and symbol presence
  LIBNAME=MdsIpShr # you may want to change this
  LIBSYMBOL=ConnectToMds # you most likely want to change this 

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $MDSPLUS_DIR/lib, MDSPLUS_SHARED_LIBADD)
    AC_DEFINE(HAVE_MDSPLUSLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong mdsplus lib version or lib not found])
  ],[
    -L$MDSPLUS_DIR/lib -lm -ldl
  ])
  dnl
  PHP_SUBST(MDSPLUS_SHARED_LIBADD)

  PHP_NEW_EXTENSION(mdsplus, mdsplus.c, $ext_shared)
fi
