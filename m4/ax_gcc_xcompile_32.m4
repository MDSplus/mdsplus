

AC_DEFUN([AX_AC_TARGET_ARCH],[
  m4_define([AC_LANG(C)],[
   ac_compile='$CC -c $CFLAGS $CPPFLAGS $TARGET_ARCH conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
   ac_link='$CC -o conftest$ac_exeext $CFLAGS $CPPFLAGS $TARGET_ARCH $LDFLAGS conftest.$ac_ext $LIBS >&AS_MESSAGE_LOG_FD'
  ])
  m4_define([AC_LANG(C++)],[
   ac_compile='$CXX -c $CXXFLAGS $CPPFLAGS $TARGET_ARCH conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
   ac_link='$CXX -o conftest$ac_exeext $CXXFLAGS $CPPFLAGS $TARGET_ARCH $LDFLAGS conftest.$ac_ext $LIBS >&AS_MESSAGE_LOG_FD'
  ])
])


AC_DEFUN([AX_GCC_XCOMPILE_32],[
  AC_REQUIRE([AX_COMPILER_VENDOR])
  AC_REQUIRE([AX_COMPILER_VERSION])
  AC_REQUIRE([AX_AC_TARGET_ARCH])
  AS_CASE(["${build}:${host}:${ax_cv_c_compiler_vendor}"],
    #
    # LINUX-GNU_64->LINUX-GNU_32 with GCC
    #
    [x86_64*linux*:i?86-*linux*:*gnu*],[
     AS_VAR_APPEND([TARGET_ARCH],[" -m32"])],
    # default
    []
  )
])


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
