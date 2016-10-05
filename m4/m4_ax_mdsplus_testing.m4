
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
AS_VAR_SET_IF([$1],,AS_VAR_SET([$1]))
for _i in $2; do
 AS_VAR_APPEND([$1], "$(shell winepath -w $_i 2>/dev/null);");
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
         dnl // if not defined HAVE_WINEPATH search for winepath in current path //
         AS_VAR_SET_IF([HAVE_WINEPATH],,[AC_CHECK_PROG(HAVE_WINEPATH,winepath,yes,no)])
         TS_VAR_YN(HAVE_WINEPATH,[
         for _i in $_libs; do
          AS_VAR_APPEND($1, "\$(shell winepath -w $_i 2>/dev/null);");
         done
         ]) dnl YN
         
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
                 [$(eval "WINEPREFIX=${WINEPREFIX}             WINEARCH=${$2}" wineboot -f -r)],
                 [$(eval "WINEPREFIX=$(pwd)/${winebottle_name} WINEARCH=${$2}" wineboot -f -i)])
   ])
  AS_VAR_SET([$1],[${with_winebottle}])
  ]) dnl HAVE_WINE
])




dnl ////////////////////////////////////////////////////////////////////////////
dnl /// TS PYTHON  /////////////////////////////////////////////////////////////
dnl ////////////////////////////////////////////////////////////////////////////

AC_DEFUN([TS_CHECK_NOSETESTS],[
  AC_CHECK_PROG([NOSETESTS], [nosetests], [nosetests])
  AS_IF(test x"${NOSETESTS}" != x"",
   [$2],
   [$3])
])

dnl TEST for python modules
AC_DEFUN([TS_CHECK_PYTHON_TAP],[
  AC_PYTHON_MODULE(tap)
  AC_PYTHON_MODULE_CLASS(tap, TAPTestRunner, [have_tappy="yes"],)
  AC_CHECK_PROG([NOSETESTS], [nosetests], [nosetests])
  AS_IF(test x"${have_tappy}" == x"yes" -a x"${NOSETESTS}" != x"",
   [$2],
   [$3])
])


AC_DEFUN([TS_SET_SKIP_CMD],[
          AS_VAR_SET([$1],["sh -c \"exit 77\"; :"])
         ])

dnl generate SKIP log_compiler
AC_DEFUN([TS_LOG_SKIP],[
 AS_VAR_APPEND([$1],["sh -c \"exit 77\"; :"])
])



dnl ////////////////////////////////////////////////////////////////////////////
dnl /// TS SELECT HOST  ////////////////////////////////////////////////////////
dnl ////////////////////////////////////////////////////////////////////////////

dnl
dnl This function select from current build_os and host target the proper env to
dnl correctly call the test chain.
dnl The test chain is composed by: [tests_env] [log_driver] [log_compiler] [test_flags]
dnl
AC_DEFUN([TS_SELECT],[
 AS_VAR_SET([LOG_DRIVER],["\$(SHELL) \$(top_srcdir)/conf/test-driver"])
 AS_VAR_SET([abs_srcdir],$(cd ${srcdir}; pwd))

 AS_VAR_IF([HAVE_PYTHON],[yes],
	   [TS_CHECK_NOSETESTS([$PYTHON],,
	      [AC_MSG_WARN("python-nose not found")])
	    TS_CHECK_PYTHON_TAP( [$PYTHON],,
	      [AC_MSG_WARN("Tap plugin for python-nose not found")])
	    AS_VAR_APPEND([PY_LOG_COMPILER],  ["${PYTHON} -B \$(top_srcdir)/testing/testing.py"])
	    AS_VAR_APPEND([PY_LOG_FLAGS], [""])
	   ],
	   [TS_LOG_SKIP([PY_LOG_COMPILER])])


 AS_CASE(["${build_os}:${host}"],
 #
 # LINUX->MINGW
 #
 [*linux*:*mingw*],
 [
   AS_VAR_SET([ENABLE_TESTS],[yes])
   AS_ECHO("Set tests environment for linux->mingw")
   AS_VAR_SET_IF([HAVE_WINE],,[AC_CHECK_PROG(HAVE_WINE,wine,yes,no)])
   AS_VAR_IF([HAVE_WINE],[yes],
     [
      TS_WINE_ENV([WINEPREFIX],[WINEARCH])
      TS_WINE_LIBRARIESPATH([WINEPATH])
      TS_WINEPATH([_mds_path],["${abs_srcdir}/tdi"])
      AS_VAR_APPEND([TESTS_ENVIRONMENT],"MDS_PATH='${_mds_path}' ")
      AS_VAR_APPEND([TESTS_ENVIRONMENT],"WINEARCH='${WINEARCH}' WINEPREFIX='${WINEPREFIX}' ")
      AS_VAR_APPEND([TESTS_ENVIRONMENT],"WINEPATH='${WINEPATH}' ")
      AS_VAR_APPEND([LOG_COMPILER],"wine ")

 # WINE Valgrind tuning ..
 # see: http://wiki.winehq.org/WineAndValgrind
 #
 # ensures that bitmap-related code, such as GetBitmapBits(), works under Valgrind
      AS_VAR_APPEND([VALGRIND_FLAGS],"--vex-iropt-register-updates=allregs-at-mem-access ")
 #
 # quiets warnings about accesses slightly below the stack pointer. This is due
 # to a known but benign piece of code in Wine's ntdll
 # (see Bug #26263 for details)
      AS_VAR_APPEND([VALGRIND_memcheck_FLAGS],"--workaround-gcc296-bugs=yes  ")
     ],
     [TS_LOG_SKIP([LOG_COMPILER])])
 ],
 #
 # LINUX->LINUX
 #
 [*linux*:*linux*],
 [
   AS_VAR_SET([ENABLE_TESTS],[yes])
   AS_ECHO("Set tests environment for linux->linux")
   AS_VAR_APPEND([TESTS_ENVIRONMENT],"MDSPLUS_DIR=\$(abs_top_srcdir) ")
   AS_VAR_APPEND([TESTS_ENVIRONMENT],"MDS_PATH=\$(abs_top_srcdir)/tdi ")
   AS_VAR_APPEND([TESTS_ENVIRONMENT],"${LIBPATH}=${MAKESHLIBDIR}\$(if \${${LIBPATH}},:\${${LIBPATH}}) ")
   AS_VAR_APPEND([TESTS_ENVIRONMENT],"PYTHONPATH=\$(prefix)/mdsobjects/python:\$(abs_top_srcdir)/mdsobjects/python:\$(abs_top_srcdir)/testing\$(if \${PYTHONPATH},:\${PYTHONPATH}) PYTHONDONTWRITEBYTECODE=yes")
 ],
 #
 # OTHER
 #
 [*],
 [
   # in all other platform tests are disabled for now
   AS_VAR_SET([ENABLE_TESTS],[no])
 ])

# MACOS: add --dsymutil=yes to valgrind


# Set ENABLE_TESTS
  AC_SUBST([ENABLE_TESTS])
  AM_CONDITIONAL([ENABLE_TESTS],[test x"${ENABLE_TESTS}" = x"yes"])
  m4_ifdef([AM_SUBST_NOTMAKE], [AM_SUBST_NOTMAKE([ENABLE_TESTS])])


])






dnl --- TEST CHAIN ---

dnl LOG_DRIVER = $(SHELL) $(top_srcdir)/conf/test-driver
dnl LOG_COMPILE = $(LOG_COMPILER) $(AM_LOG_FLAGS) $(LOG_FLAGS)
dnl PY_LOG_DRIVER = $(SHELL) $(top_srcdir)/conf/test-driver
dnl PY_LOG_COMPILE = $(PY_LOG_COMPILER) $(AM_PY_LOG_FLAGS) $(PY_LOG_FLAGS)

dnl am__test_logs1 = $(TESTS:=.log)
dnl am__test_logs2 = $(am__test_logs1:.log=.log)
dnl TEST_LOGS = $(am__test_logs2:.test.log=.log)
dnl TEST_LOG_DRIVER = $(SHELL) $(top_srcdir)/conf/test-driver
dnl TEST_LOG_COMPILE = $(TEST_LOG_COMPILER) $(AM_TEST_LOG_FLAGS) $(TEST_LOG_FLAGS)
dnl DIST_SUBDIRS = $(SUBDIRS)

dnl buildtest.log: buildtest$(EXEEXT)
dnl        @p='buildtest$(EXEEXT)'; \
dnl        b='buildtest'; \
dnl        $(am__check_pre) $(LOG_DRIVER) --test-name "$$f" \
dnl        --log-file $$b.log --trs-file $$b.trs \
dnl        $(am__common_driver_flags) $(AM_LOG_DRIVER_FLAGS) $(LOG_DRIVER_FLAGS) -- $(LOG_COMPILE) \
dnl        "$$tst" $(AM_TESTS_FD_REDIRECT)

dnl .py.log:
dnl        @p='$<'; \
dnl        $(am__set_b); \
dnl        $(am__check_pre) $(PY_LOG_DRIVER) --test-name "$$f" \
dnl        --log-file $$b.log --trs-file $$b.trs \
dnl        $(am__common_driver_flags) $(AM_PY_LOG_DRIVER_FLAGS) $(PY_LOG_DRIVER_FLAGS) -- $(PY_LOG_COMPILE) \
dnl        "$$tst" $(AM_TESTS_FD_REDIRECT)



dnl #
dnl # nosetests alternative TODO: add this with nosetests check
dnl #
dnl _tap_py_execute = \
dnl import unittest; \
dnl import tap; \
dnl import os; \
dnl import sys; \
dnl pt = os.path.dirname(os.path.curdir); \
dnl loader = unittest.TestLoader(); \
dnl tests = loader.loadTestsFromName(sys.argv[1]); \
dnl tr = tap.TAPTestRunner(); \
dnl tr.set_stream(1); \
dnl tr.run(tests);

dnl #prova:
dnl #	echo $(TEST_LOGS);
dnl #	python -c "${_tap_py_execute}" pyex1;

