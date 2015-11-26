
AC_DEFUN([TS_VAR_YN], [         
         AS_CASE([${$1}],
                 [yes|Yes|YES|Y|y],$2,
                 [no|No|NO|N|n],$3,
                 [*],$4 ) 
          ])

AC_DEFUN([TS_DEBUG_VAR],[AS_ECHO("DEBUG: $1 = ${$1}")])
 

dnl ////////////////////////////////////////////////////////////////////////////
dnl /// TS WINE    /////////////////////////////////////////////////////////////
dnl ////////////////////////////////////////////////////////////////////////////


AC_DEFUN([TS_WINEPATH],[
for _i in $1; do
 AS_VAR_APPEND($2, "\$(shell winepath -w $_i);");
done
])

dnl compile a winepath command using specified directories
dnl Usage: TS_WINEPATH [var],[library_dirs]
dnl 
AC_DEFUN([TS_WINE_LIBRARIESPATH],[         
         m4_pushdef([libdir], [m4_default([$2], [${MAKESHLIBDIR}])])
         AC_PROG_SED
         AC_PROG_GREP
         AS_VAR_SET([_libs], [$(${CC} --print-search-dir | \
           ${GREP} "libraries:" | \
           ${SED} 's/^libraries: =//' | \
           ${SED} 's/:/ /g')])          
         AS_VAR_APPEND([_libs],[" "])
         
         AS_VAR_SET([_prog], [$(${CC} --print-search-dir | \
           ${GREP} "programs:" | \
           ${SED} 's/^programs: =//' | \
           ${SED} 's/:/ /g')])         
         AS_VAR_APPEND([_libs],["${_prog} "])

         dnl this is a workaround for build_mingw (centos7) docker image
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
           AS_VAR_APPEND($1, "\$(shell winepath -w $_i);");
          done
          ]) dnl YN
         ]) dnl _ts_winepath
         
         dnl TODO:  m4_ifval(m4_normalize([cond]), if-text, if-blank)
         _ts_winepath()         
         
         m4_popdef([_ts_winepath])
         m4_popdef([libdir])
])


dnl TS_WINE_ENV [WINEPREFIX] [WINEARCH]
dnl 
AC_DEFUN([TS_WINE_ENV],[
  
  dnl select wine architecture
  AS_CASE([${host}],
          [i686*mingw*],   [AS_VAR_SET([winebottle_name],["winebottle_mingw32"]) AS_VAR_SET([$2],["win32"])],
          [x86_64*mingw*], [AS_VAR_SET([winebottle_name],["winebottle_mingw64"]) AS_VAR_SET([$2],["win64"])] )
          
  dnl select a wine bottle
  AC_ARG_WITH(winebottle,
              [AS_HELP_STRING([--with-winebottle],[specify bottle])],  
              [],
              [AS_VAR_SET_IF([winebottle],
                             [AS_VAR_SET([with_winebottle], ["${WINEPREFIX}"])],
                             [AS_VAR_SET([with_winebottle], ["\$(top_builddir)/${winebottle_name}"])])])

  dnl launch boot in wine bottle (creating new one if it does not exist)
  AS_VAR_SET_IF([HAVE_WINEBOOT],,[AC_CHECK_PROG(HAVE_WINEBOOT,wineboot,yes,no)])
  AS_VAR_IF([HAVE_WINEBOOT],[yes],
   [AS_VAR_SET_IF([WINEPREFIX],
                 [$(eval "WINEPREFIX=${WINEPREFIX}         WINEARCH=${$2}" wineboot -f -r)],
                 [$(eval "WINEPREFIX=$(pwd)/${winebottle_name} WINEARCH=${$2}" wineboot -f -i)])
   ])
  AS_VAR_SET([$1],[${with_winebottle}])
  ]) dnl HAVE_WINE
]) 




dnl ////////////////////////////////////////////////////////////////////////////
dnl /// TS PYTHON  /////////////////////////////////////////////////////////////
dnl ////////////////////////////////////////////////////////////////////////////

dnl TEST for python modules
AC_DEFUN([TS_CHECK_PYTHON_TAP],[
  AC_PYTHON_MODULE(tap)
  AC_PYTHON_MODULE_CLASS(tap, TAPTestRunner, [have_tappy="yes"],)
  AC_CHECK_PROG([NOSETESTS], [nosetests], [nosetests])  
  AS_IF(test x"${have_tappy}" == x"yes" -a x"${NOSETESTS}" != x"",
   [eval $2],
   [eval $3])
])

dnl generate SKIP log_compiler
AC_DEFUN([TS_LOG_SKIP],[":"])






dnl ////////////////////////////////////////////////////////////////////////////
dnl /// TS SELECT HOST  ////////////////////////////////////////////////////////
dnl ////////////////////////////////////////////////////////////////////////////

dnl 
dnl This function select from current build_os and host target the proper env to
dnl correctly call the test chain.
dnl The test chain is composed by: [tests_env] [log_driver] [log_compiler] [test_flags]
dnl 
AC_DEFUN([TS_SELECT],[
 AS_VAR_SET([TS_TESTS_ENVIRONMENT])
 AS_VAR_SET([TS_LOG_COMPILER])
 AS_VAR_SET([TS_PY_TAP_COMPILER])
 AS_VAR_SET([TS_PY_TAP_FLAGS])
 AS_VAR_SET([TS_LOG_DRIVER],["\$(top_srcdir)/conf/test-driver"])

 AS_VAR_SET([abs_srcdir],$(cd ${srcdir}; pwd))

 TS_CHECK_PYTHON_TAP( [$PYTHON], 
   [AS_VAR_APPEND([TS_PY_TAP_COMPILER],["${NOSETESTS}"])
    AS_VAR_APPEND([TS_PY_TAP_FLAGS],   ["--with-tap --tap-stream"])],
   AS_VAR_APPEND([TS_PY_TAP_COMPILER],[TS_LOG_SKIP]))

 dnl this calls valgrind check with args
 AX_VALGRIND_CHECK

 AS_CASE(["${build_os}:${host}"],
 #
 # LINUX->MINGW
 #
 [*linux*:*mingw*], 
 [
   AS_ECHO("Set tests environment for linux->mingw")
   AS_VAR_SET_IF([HAVE_WINE],,[AC_CHECK_PROG(HAVE_WINE,wine,yes,no)])
   AS_VAR_IF([HAVE_WINE],[yes],
     [TS_WINE_ENV([WINEPREFIX],[WINEARCH]) 
      TS_WINE_LIBRARIESPATH([WINEPATH])
      AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"MDSPLUS_DIR=${abs_srcdir} ")
      AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"MDS_PATH=${abs_srcdir}/tdi ")      
      AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"WINEARCH='${WINEARCH}' WINEPREFIX='${WINEPREFIX}' ")
      AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"WINEPATH='${WINEPATH}' ")
      AS_VAR_IF([VALGRIND_ENABLED],[yes],
       [AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"\$(VALGRIND_TESTS_ENVIRONMENT) ")
        AS_VAR_APPEND([TS_LOG_COMPILER],"\$(VALGRIND_LOG_COMPILER) ")
        AS_VAR_APPEND([TS_LOG_COMPILER],"--vex-iropt-register-updates=allregs-at-mem-access ")
        AS_VAR_APPEND([TS_LOG_COMPILER],"--workaround-gcc296-bugs=yes  ")
        AS_VAR_APPEND([TS_LOG_COMPILER],"\$(TS_VALGRIND_FLAGS)  ")
       ])
      AS_VAR_APPEND([TS_LOG_COMPILER],"wine ")
     ],
     [AS_VAR_APPEND([TS_LOG_COMPILER],[TS_LOG_SKIP])])     
     dnl TODO: add python in wine (with winetricks?)
     dnl force SKIP log for python for now
     AS_VAR_SET([TS_PY_TAP_COMPILER],[TS_LOG_SKIP])     
 ],
 #
 # LINUX->LINUX
 #
 [*linux*:*linux*],
 [
   AS_ECHO("Set tests environment for linux->linux")
   AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"MDSPLUS_DIR=${abs_srcdir} ")
   AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"MDS_PATH=${abs_srcdir}/tdi ")
   AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"${LIBPATH}=${MAKESHLIBDIR} ")
   AS_VAR_IF([VALGRIND_ENABLED],[yes],
             [AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"\$(VALGRIND_TESTS_ENVIRONMENT) ")
              AS_VAR_APPEND([TS_LOG_COMPILER],"\$(VALGRIND_LOG_COMPILER) ")
              AS_VAR_APPEND([TS_LOG_COMPILER],"\$(TS_VALGRIND_FLAGS)  ")
              ])
 ],
 #
 # OTHER
 #
 [*],
 [
   AS_ECHO("Set tests environment")  
   AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"MDSPLUS_DIR=${abs_srcdir} ")
   AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"MDS_PATH=${abs_srcdir}/tdi ")
   AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"${LIBPATH}=${MAKESHLIBDIR} ")
   AS_VAR_IF([VALGRIND_ENABLED],[yes],
             [AS_VAR_APPEND([TS_TESTS_ENVIRONMENT],"\$(VALGRIND_TESTS_ENVIRONMENT) ")
              AS_VAR_APPEND([TS_LOG_COMPILER],"\$(VALGRIND_LOG_COMPILER) ")
              AS_VAR_APPEND([TS_LOG_COMPILER],"\$(TS_VALGRIND_FLAGS)  ")
              ])
 ])

# MACOS: add --dsymutil=yes to valgrind 
 

])





dnl --------------------------------------------------------------------------
dnl -- ONLY FOR TESTING M4 ---------------------------------------------------
dnl --------------------------------------------------------------------------

AC_DEFUN([TS_TEST],[
AS_ECHO( --- )

 TS_WINEPATH([WINEPATH],[/bin,/lib])
 TS_DEBUG_VAR(WINEPATH) 
dnl TS_SELECT()
dnl TS_DEBUG_VAR(WINEPATH)
dnl TS_DEBUG_VAR(TS_TESTS_ENVIRONMENT)

AS_ECHO( --- )
])
