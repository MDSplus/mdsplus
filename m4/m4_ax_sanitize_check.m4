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
                                AS_VAR_SET([sanitize_libpath], [$($LDD conftest$EXEEXT | grep [lib]_xsan[].so | $AWK '{ print $[]3 }')])])
                AS_VAR_SET_IF([sanitize_libpath],[AC_MSG_NOTICE([found library path for] _xsan: $sanitize_libpath)],
                                                 [AC_MSG_WARN([library path not found for] _xsan)])
                AS_VAR_SET([CPPFLAGS],[${CPPFLAGS_save}])
                AS_VAR_SET([LIBS],[${LIBS_save}])
                [$3]
               ],
               [$4])     
     m4_popdef([_xsan])
     m4_popdef([_XSAN])
])


#   AX_ENABLE_SANITIZE([variable name],[action if sanitizer found],
#                      [action if sanitizer not found])
AC_DEFUN([AX_ENABLE_SANITIZE],[
  m4_pushdef([ax_enable_sanitize],m4_default($1,[ENABLE_SANITIZE]))

  AC_ARG_ENABLE([sanitize],
    [AS_HELP_STRING([--enable-sanitize=flavor],
      [Enable compile sanitizer with flavor in (address | thread | undefined) [[default=address]]])
    ],
    [AS_VAR_IF([enableval],[yes],
               [AS_VAR_SET([enable_sanitize],[address])],
               [AS_VAR_SET([enable_sanitize],[$enableval])])],             
    [AS_VAR_SET_IF(ax_enable_sanitize,,
                   [AS_VAR_SET([enable_sanitize],[no])])]  
  )
 
  if [ test x"$enable_sanitize" != x"no" -a "$GCC" = "yes" ]; then
     AX_COMPILER_VERSION
     AX_COMPARE_VERSION([${ax_cv_c_compiler_version}], [ge], [4.8.0],,
		     [AC_MSG_RESULT([gcc version 4.8.0 or higher required for sanitizers])
		      $3
		     ])
   
     AS_CASE([${enable_sanitize}],
     # address sanitizer
     [address],
     [AX_SANITIZER_CHECK([asan],["-fsanitize=address -fno-omit-frame-pointer"],
                         [sanitizer_ok=yes],[$3])],
     # thread sanitizer
     [thread],     
     [AX_SANITIZER_CHECK([tsan],["-fsanitize=thread -fno-omit-frame-pointer"],
                         [sanitizer_ok=yes],[$3])],
     # undefined sanitizer
     [undefined],
     [AX_SANITIZER_CHECK([ubsan],["-fsanitize=undefined -fno-omit-frame-pointer"],
                         [sanitizer_ok=yes],[$3])],
     # default   
     [$3])
  fi

  AM_CONDITIONAL(ax_enable_sanitize, [test "${enable_sanitize}" != "no"] )
  AS_VAR_IF([sanitizer_ok],[yes],
            [AS_VAR_SET(ax_enable_sanitize,[${enable_sanitize}])
             AC_SUBST(ax_enable_sanitize,[${enable_sanitize}])
             AC_SUBST(ax_enable_sanitize[]_OPTIONS,[${sanitize_env}])  
             AC_SUBST(ax_enable_sanitize[]_LIBPATH,[${sanitize_libpath}])
             $2
            ])


  m4_popdef([ax_enable_sanitize])
])



