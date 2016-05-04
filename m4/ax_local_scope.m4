

AC_DEFUN([AC_DEFUN_LOCAL],
         [AC_DEFUN([$1_$2],[$3])
          m4_append_uniq([$1_FUNCS],[[[$2]]],[,])])

AC_DEFUN([AC_PUSH_LOCAL],
         [m4_foreach([func],[$1_FUNCS],[m4_pushdef(func,m4_defn($1_[]func))])])

AC_DEFUN([AC_POP_LOCAL],
         [m4_foreach([func],[$1_FUNCS],[m4_popdef(func)])])





