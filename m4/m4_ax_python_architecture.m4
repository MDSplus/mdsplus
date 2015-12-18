
AC_DEFUN([AX_PYTHON_ARCH], [
    AX_PYTHON_MODULE([platform], [required], [${PYTHON}])
    AC_MSG_CHECKING([for python architecture])
    AS_VAR_SET([_python_arch],
               [`${PYTHON} -c "import sys, platform; sys.stdout.write(platform.architecture()[[0]])"`])
    
    AS_VAR_IF([_python_arch],[64bit],[
               ${PYTHON} -c "import sys; sys.exit(sys.maxsize <= 2**32)"
               AS_IF([test $? -eq 0],[],[
               AS_VAR_SET([_python_arch],[32bit])])
              ])
     
    AC_MSG_RESULT([${_python_arch}])
    AS_VAR_SET([$1],[${_python_arch}])
])
