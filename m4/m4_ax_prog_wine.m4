

dnl FINIRE !!!

dnl compile a winepath command using specified directories
dnl Usage: AX_WINE_PATH [var],[library_dirs],[search_dirs]
dnl 
AC_DEFUN([AX_WINE_PATH],[         
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

         dnl this is a workaround for build_mingw (centos7) docker image
         AS_VAR_SET([_sysroot],[$(${CC} --print-sysroot)])
         AS_VAR_APPEND([_libs],["${_sysroot}/mingw/bin "])
         AS_VAR_APPEND([_libs],["${_sysroot}/mingw/lib "])
         AS_VAR_APPEND([_libs],m4_join([" "],libdir))
          
         AS_VAR_SET($1)
         m4_pushdef([_ts_winepath],[
          dnl // if not defined HAVE_WINEPATH search for winepath in current path //
          AS_VAR_SET_IF([HAVE_WINEPATH],,[AC_CHECK_PROG(HAVE_WINEPATH,winepath,yes,no)])
          AS_VAR_if([HAVE_WINEPATH],[yes],[
            for _i in $_libs; do
              AS_VAR_APPEND($1, "\$(shell winepath -w $_i);");
            done
            ]) dnl HAVE_WINEPATH
         ]) dnl _ts_winepath
         
         dnl TODO:  m4_ifval(m4_normalize([cond]), if-text, if-blank)
         _ts_winepath()         
         
         m4_popdef([_ts_winepath])
         m4_popdef([libdir])
])


dnl AX_WINE_ENV [WINEPREFIX] [WINEARCH]
dnl 
AC_DEFUN([AX_WINE_ENV],[
  
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
                 [$(eval "WINEPREFIX=${WINEPREFIX} WINEARCH=${$2}" wineboot -f -r)],
                 [$(eval "WINEPREFIX=$(pwd)/${winebottle_name} WINEARCH=${$2}" wineboot -f -i)])
   ])
  AS_VAR_SET([$1],[${with_winebottle}])
  ]) dnl HAVE_WINE
]) 

