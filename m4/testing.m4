
AC_DEFUN([TS_VAR_YN], [         
         AS_CASE([${$1}],
                 [yes|Yes|YES|Y|y],$2,
                 [no|No|NO|N|n],$3,
                 [*],$4 ) 
          ])

AC_DEFUN([TS_DEBUG_VAR],[AS_ECHO("DEBUG: $1 = ${$1}")])
 

dnl compile a winepath command using specified directories
dnl Usage: TS_WINEPATH [var],[library_dirs],[search_dirs]
AC_DEFUN([TS_WINEPATH],[         
         m4_pushdef([libdir], [m4_default([$2], [${MAKESHLIBDIR}])])
         
         AS_VAR_SET([_libs], [$(${CC} --print-search-dir | \
           grep "libraries:" | \
           sed 's/^libraries: =//' | \
           sed 's/:/ /g')])          
         AS_VAR_APPEND([_libs],[" "])
         AS_VAR_SET([_prog], [$(${CC} --print-search-dir | \
           grep "programs:" | \
           sed 's/^programs: =//' | \
           sed 's/:/ /g')])         
         AS_VAR_APPEND([_libs],["${_prog} "])

         dnl 
         dnl this is a workaround for build_mingw (centos7) docker image
         dnl 
         AS_VAR_SET([_sysroot],[$(${CC} --print-sysroot)])
         AS_VAR_APPEND([_libs],["${_sysroot}/mingw/bin "])
         AS_VAR_APPEND([_libs],["${_sysroot}/mingw/lib "])         
         AS_VAR_APPEND([_libs],m4_join([" "],libdir))
          
         AS_VAR_SET($1)
         m4_pushdef([_ts_winepath],[
          dnl // if not defined HAVE_WINEPATH search for winepath in current path //
          AS_VAR_SET_IF([HAVE_WINEPATH],,[AC_CHECK_PROG(HAVE_WINEPATH,winepath,yes,no)])
          TS_VAR_YN(HAVE_WINEPATH,[
          for _i in $_libs; do
           dnl AS_ECHO(" --> $_i")
           AS_VAR_APPEND($1, "\$(shell winepath -w $_i);");
          done         
          dnl  AS_VAR_APPEND($1,m4_map_args_sep(["\$(shell winepath -w "], [")"], [";"], gwdir))
          dnl  AS_VAR_APPEND($1,m4_map_args_sep(["\$(shell winepath -w "], [")"], [";"], libdir))
          ]) dnl YN
         ]) dnl _ts_winepath
         
         dnl TODO:  m4_ifval(m4_normalize([cond]), if-text, if-blank)
         _ts_winepath()
         
         m4_popdef([_ts_winepath])
         m4_popdef([libdir])
])

dnl TODO: add wine arch selection bottle
AC_DEFUN([TS_WINEARCH],[]) 
AC_DEFUN([TS_WINEPREFIX],[])
 
AC_DEFUN([TS_WINE],[])



AC_DEFUN([TS_SELECT],[
 AS_VAR_SET([TS_TESTS_ENVIRONMENT])
 AS_VAR_SET([TS_LOG_COMPILER])
dnl AS_VAR_SET([TS_LOG_DRIVER],["$(top_srcdir/conf/test-driver)"])

 case "${build_os}:${host}" in

#( LINUX->MINGW
 *linux*:*mingw*)
   AS_ECHO("Set tests environment for linux->mingw")
   TS_WINEPATH([WINEPATH])
   AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"WINEPATH='${WINEPATH}' ")
   AS_VAR_APPEND([TS_LOG_COMPILER],"wine ")
 ;; 

#( LINUX->LINUX
 *linux*:*linux*)
   AS_ECHO("Set tests environment for linux->linux")
   AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"${LIBPATH}=${MAKESHLIBDIR} ")
 ;;

#( OTHER
 *)
   AS_ECHO("Set tests environment for unknown conf")  
 ;;

esac
])


dnl finire ....
dnl AC_DEFUN([TS_CONFIG],[
dnl  TESTS_ENVIRONMENT=${TS_TESTS_ENVIRONMENT}
dnl  LOG_COMPILER=${TS_LOG_COMPILER}
dnl  LOG_DRIVER=${TS_LOG_DRVER}
dnl  AC_SUBST([TS_CONFIG])
dnl ])


AC_DEFUN([TS_TEST],[
AS_ECHO( --- )

dnl get_gwdir()

 TS_WINEPATH([WINEPATH],[/bin,/lib])
 TS_DEBUG_VAR(WINEPATH)

dnl TS_SELECT()
dnl TS_DEBUG_VAR(WINEPATH)
dnl TS_DEBUG_VAR(TS_TESTS_ENVIRONMENT)

AS_ECHO( --- )
])
