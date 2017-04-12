#
# SYNOPSIS
#
#   AX_ENABLE_SANITIZE([variable name],[action if sanitizer found],
#                      [action if sanitizer not found])
#
# DESCRIPTION
#


#serial 1


# AX_CHECK_SANITIZE([name],[flags],[action if found],[action if not found])
# if found defines: sanitize_flags sanitize_libs sanitize_env sanitize_libpath
#
AC_DEFUN([AX_SANITIZER_CHECK],[
     m4_pushdef([_xsan],m4_tolower($1))
     m4_pushdef([_XSAN],m4_toupper($1))

     LDD=ldd
     AC_CHECK_LIB(_xsan,[_init],[have_[]_xsan=yes])
     AS_VAR_IF([have_[]_xsan],[yes],
               [AS_VAR_SET([CPPFLAGS_save],[$CPPFLAGS])
                AS_VAR_SET([LIBS_save],[$LIBS])
                AS_VAR_SET([CPPFLAGS],[$2])
                AS_VAR_SET([LIBS],[-l[]_xsan])
                AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>]],
                                                [[int x; (void)x;]])],
                               [AS_VAR_SET([sanitize_flags],[$CPPFLAGS])
                                AS_VAR_SET([sanitize_libs],[$LIBS])
                                AS_VAR_SET([sanitize_env],[_XSAN[]_OPTIONS])
                                AS_VAR_SET([sanitize_libpath], [$($LDD conftest$EXEEXT \
                                 | grep [lib]_xsan[].so | $AWK '{ print $[]3 }')])
                                AS_VAR_SET([have_[]_xsan],[yes])],
                               [AS_VAR_SET([have_[]_xsan],[no])])
                AS_VAR_SET_IF([sanitize_libpath],
                              [AC_MSG_NOTICE([found library path for] _xsan: $sanitize_libpath)],
                              [AC_MSG_WARN([library path not found for] _xsan)])
                AS_VAR_SET([CPPFLAGS],[${CPPFLAGS_save}])
                AS_VAR_SET([LIBS],[${LIBS_save}])])
     AS_VAR_IF([have_[]_xsan],[yes],[$3],[$4])
     m4_popdef([_xsan])
     m4_popdef([_XSAN])
])


#   AX_ENABLE_SANITIZE([variable name],[action if sanitizer found],
#                      [action if sanitizer not found])
AC_DEFUN([AX_ENABLE_SANITIZE],[
  m4_pushdef([ax_enable_sanitize],m4_default($1,[ENABLE_SANITIZE]))

  AC_ARG_ENABLE([sanitize],
    [AS_HELP_STRING([--enable-sanitize=flavor],
      [Enable compile sanitizer with flavor in (address | thread | undefined) [[default=address]] ])],
    [AS_VAR_SET([enable_sanitize],[${enableval}])],
    [AS_VAR_SET_IF(ax_enable_sanitize,
                   [AS_VAR_SET([enable_sanitize],${ax_enable_sanitize})],
                   [AS_VAR_SET([enable_sanitize],[no])])])

  # by default yes is the address sanitizer
  AS_VAR_IF([enable_sanitize],[yes],
            [AS_VAR_SET([enable_sanitize],[address])])

  if [ test x"$enable_sanitize" != x"no" -a "$GCC" = "yes" ]; then
     AX_COMPILER_VERSION
     AC_MSG_CHECKING([for Sanitize-able gcc version])
     AX_COMPARE_VERSION([${ax_cv_c_compiler_version}], [ge], [4.8.0],
                     [AC_MSG_RESULT([yes])],
		     [AC_MSG_RESULT([no, gcc 4.8.0 or higher required])
		      AS_VAR_SET([enable_sanitize],[no])])

     AS_CASE([${enable_sanitize}],
     # address sanitizer
     [address],
     [AX_SANITIZER_CHECK([asan],["-fsanitize=address -fno-omit-frame-pointer -O3"],,
     [AS_VAR_SET([enable_sanitize],[no])])],
     # thread sanitizer
     [thread],
     [AX_SANITIZER_CHECK([tsan],["-fsanitize=thread -fno-omit-frame-pointer -O3"],,
     [AX_SANITIZER_CHECK([tsan],["-fsanitize=thread -fno-omit-frame-pointer -fPIE -O3"],,
     [AS_VAR_SET([enable_sanitize],[no])])])],
     # undefined sanitizer
     [undefined],
     [AX_SANITIZER_CHECK([ubsan],["-fsanitize=undefined -fno-omit-frame-pointer -O3"],,
     [AS_VAR_SET([enable_sanitize],[no])])],
     # default
     [no],,
     [AC_MSG_WARN([sanitizer flavor ${enable_sanitize} not found])
      AS_VAR_SET([enable_sanitize],[no])])

     AS_VAR_IF([enable_sanitize],[no],$3)
  fi

  AS_VAR_IF([enable_sanitize],[no],
            [AS_VAR_SET(ax_enable_sanitize,[no])
             AS_VAR_SET(ax_enable_sanitize[]_OPTIONS,[SAN_OPTIONS])
             AS_VAR_SET(ax_enable_sanitize[]_LIBPATH)],
            [AS_VAR_SET(ax_enable_sanitize,[${enable_sanitize}])
             AS_VAR_SET(ax_enable_sanitize[]_OPTIONS,[${sanitize_env}])
             AS_VAR_SET(ax_enable_sanitize[]_LIBPATH,[${sanitize_libpath}])
             $2 ])

  AM_CONDITIONAL(ax_enable_sanitize, [test "${enable_sanitize}" != "no"] )
  AC_SUBST(ax_enable_sanitize)
  AC_SUBST(ax_enable_sanitize[]_OPTIONS)
  AC_SUBST(ax_enable_sanitize[]_LIBPATH)

  m4_popdef([ax_enable_sanitize])
])


