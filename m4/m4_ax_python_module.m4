# ===========================================================================
#     http://www.gnu.org/software/autoconf-archive/ax_python_module.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PYTHON_MODULE(modname[, fatal, python])
#
# DESCRIPTION
#
#   Checks for Python module.
#
#   If fatal is non-empty then absence of a module will trigger an error.
#   The third parameter can either be "python" for Python 2 or "python3" for
#   Python 3; defaults to Python 3.
#
# LICENSE
#
#   Copyright (c) 2008 Andrew Collier
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AU_ALIAS([AC_PYTHON_MODULE], [AX_PYTHON_MODULE])
AC_DEFUN([AX_PYTHON_MODULE],[
    if test -z $PYTHON;
    then
        if test -z "$3";
        then
            PYTHON="python3"
        else
            PYTHON="$3"
        fi
    fi
    PYTHON_NAME=`basename $PYTHON`
    AC_MSG_CHECKING($PYTHON_NAME module: $1)
    $PYTHON -c "import $1" 2>/dev/null
    if test $? -eq 0;
    then
        AC_MSG_RESULT(yes)
        eval AS_TR_CPP(HAVE_PYMOD_$1)=yes
    else
        AC_MSG_RESULT(no)
        eval AS_TR_CPP(HAVE_PYMOD_$1)=no
        #
        if test -n "$2"
        then
            AC_MSG_ERROR(failed to find required module $1)
            exit 1
        fi
    fi
])


dnl AC_PYTHON_CLASS(module,class,[action-if-true],[action-if-false],[python])
AU_ALIAS([AC_PYTHON_MODULE_CLASS],[AX_PYTHON_MODULE_CLASS])
AC_DEFUN([AX_PYTHON_MODULE_CLASS],[
        AX_PYTHON_MODULE($1,,$5)
        AC_MSG_CHECKING(python class $1.$2)        
        AS_IF([ test "$PYTHON" == : ],
              [AC_MSG_ERROR(no python executable was found); AS_EXIT],)

        $PYTHON -c "import inspect,$1; exit(inspect.isclass($1.$2) != True);" 2>/dev/null
        AS_IF([test $? -eq 0],
              [AC_MSG_RESULT(yes); eval $3],
              [AC_MSG_RESULT(no);  eval $4])
])
