


AC_DEFUN([AX_GCC_XCOMPILE_32],[
  AC_REQUIRE([AX_COMPILER_VENDOR])
  AC_REQUIRE([AX_COMPILER_VERSION])
  AS_CASE(["${build}:${host}:${ax_cv_c_compiler_vendor}"],
    #
    # LINUX-GNU_64->LINUX-GNU_32 with GCC
    #
    [x86_64*linux*:i?86-*linux*:*gnu*],[
     AS_VAR_APPEND([CC],[" -m32"])
     AS_VAR_APPEND([CXX],[" -m32"])],
    # default
    []
  )
])


dnl # AC_LANG(C)
dnl # ----------
dnl # CFLAGS is not in ac_cpp because -g, -O, etc. are not valid cpp options.
dnl AC_LANG_DEFINE([C], [c], [C], [CC], [],
dnl [ac_ext=c
dnl ac_cpp='$CPP $CPPFLAGS'
dnl ac_compile='$CC -c $CFLAGS $CPPFLAGS conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
dnl ac_link='$CC -o conftest$ac_exeext $CFLAGS $CPPFLAGS $LDFLAGS conftest.$ac_ext $LIBS
dnl ac_compiler_gnu=$ac_cv_c_compiler_gnu
dnl ])

dnl # AC_LANG(C++)
dnl # ------------
dnl # CXXFLAGS is not in ac_cpp because -g, -O, etc. are not valid cpp options.
dnl AC_LANG_DEFINE([C++], [cxx], [CXX], [CXX], [C],
dnl [ ac_ext=cpp
dnl ac_cpp='$CXXCPP $CPPFLAGS'
dnl ac_compile='$CXX -c $CXXFLAGS $CPPFLAGS conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
dnl ac_link='$CXX -o conftest$ac_exeext $CXXFLAGS $CPPFLAGS $LDFLAGS conftest.$ac_ext $LIBS
dnl ac_compiler_gnu=$ac_cv_cxx_compiler_gnu
dnl ])

dnl //
dnl // EXAMPLE config.status file
dnl // (fc23 docker configured for i386-linux)
dnl //
dnl  S["target_os"]="linux-gnu"
dnl  S["target_vendor"]="pc"
dnl  S["target_cpu"]="i386"
dnl  S["target"]="i386-pc-linux-gnu"
dnl  S["host_os"]="linux-gnu"
dnl  S["host_vendor"]="pc"
dnl  S["host_cpu"]="i386"
dnl  S["host"]="i386-pc-linux-gnu"
dnl  S["build_os"]="linux-gnu"
dnl  S["build_vendor"]="unknown"
dnl  S["build_cpu"]="x86_64"
dnl  S["build"]="x86_64-unknown-linux-gnu"
