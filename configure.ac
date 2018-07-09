dnl Process this file with autoconf to produce a configure script.
dnl @(#)$RCSfile$ $Revision$
AC_PREREQ([2.69])
AC_INIT([MDSplus], [7a], [mdsplus@lists.psfc.mit.edu])
AC_CONFIG_AUX_DIR([conf])
AC_CONFIG_SRCDIR([include/mdsdescrip.h])

dnl docker helpers
AX_DOCKER_BUILD

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.15 foreign subdir-objects])
AM_MAINTAINER_MODE


dnl AS_IS_RELEASE //////////////////////////////////////////////////////////////
dnl see: http://www.gnu.org/software/autoconf-archive/ax_is_release.html
dnl
dnl * git-directory:  ax_is_release will be 'no' if a '.git' directory exists
dnl * minor-version:  ax_is_release will be 'no' if the minor version number
dnl                   in $PACKAGE_VERSION is odd; this assumes
dnl                   $PACKAGE_VERSION follows the 'major.minor.micro' scheme
dnl * micro-version:  ax_is_release will be 'no' if the micro version number
dnl                   in $PACKAGE_VERSION is odd; this assumes
dnl                   $PACKAGE_VERSION follows the 'major.minor.micro' scheme
dnl * always:         ax_is_release will always be 'yes'
dnl * never:          ax_is_release will always be 'no'
dnl
AX_IS_RELEASE([always])


dnl AX_CHECK_ENABLE_DEBUG //////////////////////////////////////////////////////
dnl see: www.gnu.org/software/autoconf-archive/ax_check_enable_debug.html
dnl
dnl Check for the presence of an --enable-debug option to configure, with the
dnl specified default value used when the option is not present. Return the
dnl value in the variable $ax_enable_debug.
dnl Specifying "yes" adds "-g -O0" to the compilation flags for all languages.
dnl Specifying "info" adds "-g" to the compilation flags. Specifying "profile"
dnl adds "-g -pg" to the compilation flags, and "-pg" to the linker flags.
dnl Otherwise, nothing is added.
dnl
dnl AX_CHECK_ENABLE_DEBUG([enable by default=yes/info/profile/no],
dnl   [ENABLE DEBUG VARIABLES ...], [DISABLE DEBUG VARIABLES NDEBUG ...],
dnl   [IS-RELEASE],[DEFAULT_RELEASE_FLAGS])
AX_CHECK_ENABLE_DEBUG([no],,,,[-g -O3])


dnl AX_TARGET_SELFHELP /////////////////////////////////////////////////////////
AX_TARGET_SELFHELP

AC_ARG_ENABLE([java_only],
              [AS_HELP_STRING([--enable-java_only],
                              [Only build java jar files])],
              [AS_VAR_SET([JAVA_ONLY],[yes])])

AC_ARG_WITH([jars],
	    [AS_HELP_STRING([--with-jars=directory],
                            [specify directory tree to find java jars])],
	    [AS_VAR_SET([JARS_DIR],[$withval])],
	    [AS_VAR_SET([JARS_DIR],[])])

AM_CONDITIONAL([USE_JARS_DIR],[test x"$JARS_DIR" != x])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_FC
AC_PROG_RANLIB
AC_PROG_MKDIR_P
AC_PROG_INSTALL
AC_PROG_SED
AC_CHECK_TOOL([WINDRES], [windres], [:])

# Always do the visibility check but don't set AM_CFLAGS on Windows.
# This way things get set properly even on Windows.
AC_MSG_RESULT([before VISIBILITY])
gl_VISIBILITY
AC_MSG_RESULT([**********VISIBILITY*********$CFLAG_VISIBILITY*******************])
CFLAGS="$CFLAGS $CFLAG_VISIBILITY"
#if test -n "$CFLAG_VISIBILITY" && test "$is_w32" != yes; then
#	CFLAGS="$CFLAGS $CFLAG_VISIBILITY"
#fi

#FIXME: Remove this when Makefile.inc goes away
#AS_CASE(["$MKDIR_P"],
#        [*install-sh*], [MKDIR_P="\$(top_srcdir)/$MKDIR_P"])

dnl See if we need to use native compilers
AS_CASE(["$host"],
        [sparc-sun-solaris2*], [${CC=cc}])

#AC_CONFIG_HEADER(include/mdsplus/mdsconfig.h)
AC_CHECK_SIZEOF(int *)
AC_SYS_LARGEFILE

JAVACFLAGS="-encoding UTF-8"
FOR_LINKSHARED=""

AC_MSG_CHECKING([whether to build shared libraries])
AC_ARG_ENABLE([shared],
              [AS_HELP_STRING([--disable-shared],
                              [Prevent building shared libraries])],
              [],
              [enable_shared=yes])

LINKWHOLEON=
LINKWHOLEOFF=
AS_CASE([$enable_shared],
        [yes], [AS_CASE([$host],
                        [*apple*], [LINKWHOLEON=-Wl,-force_load],
                        [*linux*], [LINKWHOLEON=-Wl,--whole-archive;LINKWHOLEOFF=-Wl,--no-whole-archive])])
AC_SUBST([LINKWHOLE])
AC_MSG_RESULT([$enable_shared])


dnl See if the linker supports -rpath-link
AC_MSG_CHECKING([whether the linker supports -rpath-link])
AS_VAR_SET_IF([LDFLAGS], [AS_VAR_SET([save_LDFLAGS], ["$LDFLAGS"])])
AS_VAR_SET([LDFLAGS], [-Wl,-rpath-link=dummy])
AC_LINK_IFELSE([AC_LANG_PROGRAM([], [])],
               [AS_VAR_SET([RPATHLINK], [-Wl,-rpath-link])])
AM_CONDITIONAL([RPATHLINK], [AS_VAR_TEST_SET([RPATHLINK])])
AC_SUBST([RPATHLINK])
AS_UNSET([LDFLAGS])
AS_VAR_SET_IF([save_LDFLAGS], [AS_VAR_SET([LDFLAGS], ["$save_LDFLAGS"])])
AS_UNSET([save_LDFLAGS])
AM_COND_IF([RPATHLINK], [AC_MSG_RESULT([yes])], [AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether to enable statistics])
AC_ARG_ENABLE([perf],
              [AS_HELP_STRING([--enable-perf],
                              [enable MDSplus I/O statistics])],
              [],
              [AS_VAR_SET([enable_perf], [no])])
AS_CASE([$enable_perf],
        [yes], [AC_DEFINE([USE_PERF], [], [Define if you want to collect performance statistics -- linux only])])
AC_MSG_RESULT([$enable_perf])

AC_MSG_CHECKING([whether to build the java components])
AC_ARG_ENABLE([java],
              [AS_HELP_STRING([--disable-java],
                              [Do not build java libraries and applications])],
              [],
              [AS_VAR_SET([enable_java], [yes])])

dnl # Add address sanitizer options to flags, if requested. Only useful for GCC
dnl # version 4.8 and later.
dnl AC_ARG_ENABLE([sanitize-address],
dnl  [AS_HELP_STRING(
dnl    [--enable-sanitize-address],
dnl    [Enable memory error detection using address sanitizer @<:@default=no@:>@]
dnl  )],
dnl  [enable_san_a="$enableval"],
dnl  [enable_san_a="no"]
dnl )

dnl AC_ARG_ENABLE([sanitize-undefined],
dnl  [AS_HELP_STRING(
dnl    [--enable-sanitize-undefined],
dnl    [Enable undefined behavior detection using undefined sanitizer @<:@default=no@:>@]
dnl  )],
dnl  [enable_san_u="$enableval"],
dnl  [enable_san_u="no"]
dnl )

dnl AC_ARG_ENABLE([sanitize-thread],
dnl  [AS_HELP_STRING(
dnl    [--enable-sanitize-address],
dnl    [Enable race condition detection using thread sanitizer @<:@default=no@:>@]
dnl  )],
dnl  [enable_san_t="$enableval"],
dnl  [enable_san_t="no"]
dnl )

dnl AC_ARG_ENABLE([universal],
dnl              [AS_HELP_STRING([--enable-universal],
dnl                              [Build MacOSX Universal Binary])],
dnl              [],
dnl              [AS_VAR_SET([enable_universal], [no])])
dnl
dnl this new java section hoses macos x... why not use the autoconf java macros?
dnl I'm faking a disable so I can enable by hand later...
dnl AS_CASE([$host_vendor],
dnl        [apple], dnl the java section breaks with macos x due to different structure
dnl                 dnl this disables it for now..  is there a AC_PROG_JAVA?
dnl                 [AS_VAR_SET([enable_java], [no])]
dnl                 [AS_VAR_IF([enable_universal], [yes], [CFLAGS="-O3 -g -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc"]
dnl                                                       [LDFLAGS="-Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc"]
dnl                                                       [F77=fort77])])
dnl AC_MSG_RESULT([$enable_java])


# //////////////////////////////////////////////////////////////////////////// #
# ///  JDK   ///////////////////////////////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #

dnl This is a workaround to maintain both JAVA_HOME and JDK_DIR env
AS_VAR_SET_IF([JDK_DIR],
              [AS_VAR_SET([JAVA_HOME],[${JDK_DIR}])])
AC_CHECK_JAVA_HOME
AS_VAR_SET([JDK_DIR],[${JAVA_HOME}])

AC_ARG_WITH([jdk],
            [AS_HELP_STRING([--with-jdk=JDKDIR],
                            [specify location of java jdk])],
            [],
            [AS_VAR_SET_IF([JDK_DIR],
                           [AS_VAR_SET([with_jdk], [$JDK_DIR])],
                           [AS_VAR_SET([with_jdk], [/etc/alternatives/java_sdk])])])

AC_ARG_WITH([java_target],
	    [AS_HELP_STRING([--with-java_target=version],
                            [specify version of compiled mdsobjects classes (i.e. 6)])],
            [AS_VAR_SET([JAVAVERSION], ["-source $withval -target $withval"])],
            [AS_VAR_SET([JAVAVERSION], [])])

AC_ARG_WITH([java_bootclasspath],
	    [AS_HELP_STRING([--with-java_bootclasspath=bootclasspath],
                            [specify the bootclasspath option for use with --with-java_target (i.e. /usr/lib/jvm/java-1.6.0-openjdk-1.6.0.0/jre/lib/rt.jar)])],
            [AS_VAR_APPEND([JAVAVERSION], [" -bootclasspath $withval"])],
            [AS_VAR_APPEND([JAVAVERSION], [])])
AC_SUBST([JAVAVERSION])

AS_CASE([$enable_java],
        [yes], [AC_MSG_CHECKING([for presence of Java Development Kit])
                AS_IF([test ! -d $with_jdk/jre],
                      [AC_MSG_RESULT([not found])
                       AC_MSG_WARN([YOU NEED TO SPECIFY THE JAVA JDK DIRECTORY OR DISABLE JAVA])
                       AC_MSG_WARN([The jdk directory can be specified by either defining a JDK_DIR environment])
                       AC_MSG_WARN([variable or by using the --with-jdk=DIR option.])
                       AC_MSG_WARN([You can disable java by using the --disable-java option.])
                       AS_EXIT])
                AC_MSG_RESULT([$with_jdk])
                AC_PATH_PROG([JAVAC], [javac], [], [$with_jdk/bin$PATH_SEPARATOR$PATH])
                AC_PATH_PROG([JAR], [jar], [], [$with_jdk/bin$PATH_SEPARATOR$PATH])
                AC_PATH_PROG([RMIC], [rmic], [], [$with_jdk/bin$PATH_SEPARATOR$PATH])])




AM_CONDITIONAL([GLOBUSLICENSE], [test -r $exec_prefix/GLOBUS_LICENSE])

AC_ARG_WITH([gsi],
            [AS_HELP_STRING([--with-gsi=globus-location:flavor],
                            [Use globus gsi for mdsip communications])],
            [],
            [AS_VAR_SET([with_gsi], [no])])
AS_CASE([$with_gsi],
        [no], [AS_VAR_SET([MDSTCPIP], [mdstcpip])],
        [*pthr*], [AC_MSG_WARN([Do NOT INCLUDE pthr in the globus flavor!])
                   AS_EXIT],
        [MDSTCPIP="mdstcpip roam"
         GLOBUS_LOCATION=`echo $withval | awk -F: '{print $1}'`
         GLOBUS_FLAVOR=`echo $withval | awk -F: '{print $2}'`
         AS_VAR_IF([GLOBUS_FLAVOR], [],
                   [AC_MSG_WARN([YOU NEED TO PROVIDE GLOBUS_LOCATION and FLAVOR when using the --with-gsi option])
                    AC_MSG_WARN([Format:   --with-gsi=globus-install-directory:flavor])
                    AC_MSG_WARN([Example:  --with-gsi=/usr/local/gt3:gcc32dbg])
                    AC_MSG_WARN([])
                    AC_MSG_WARN([Note do not include pthr in the flavor as it is automatically added.])
                    AS_EXIT])])

AC_MSG_CHECKING([for an srb installation])
AC_ARG_WITH([srb],
            [AS_HELP_STRING([--with-srb=srb-install-dir],
                            [Enable srb capabilities])],
            [],
            [AS_VAR_SET([with_srb], [no])])
AS_CASE([$with_srb],
        [no], [],
        [yes], [AC_MSG_WARN([YOU NEED TO PROVIDE THE LOCATION OF THE unixIO subdirectory of your srb installation])
                AC_MSG_WARN([when using the --with-srb option])
                AC_MSG_WARN([])
                AC_MSG_WARN([Format:   --with-srb=srb-install-directory])
                AC_MSG_WARN([Example:  --with-srb=/scratch/slocal/testc/SRB3_2client/unixIO])
                AS_EXIT],
        [SRBLIB="-L${with_srb} -lsrbUio"
         SRBINCLUDE="-I${with_srb}"
         AC_DEFINE(SRB,[],["Define if you are using SRB"])])
AC_MSG_RESULT([$with_srb])

dnl Look for labview include file
AC_MSG_CHECKING([for labview])
AC_ARG_WITH([labview],
            [AS_HELP_STRING([--with-labview=labview-top-dir],
                            [specify location of labview such as /usr/local/natinst/Labview])],
            [],
            [with_labview=yes])

AS_CASE([$with_labview],
        [yes],
        [AS_VAR_SET_IF([LABVIEW_DIR],
            [with_labview=$LABVIEW_DIR],
            [AS_IF([${srcdir}/conf/update_submodules && test -r ${srcdir}/3rd-party-apis/labview],
                   [with_labview=\${top_srcdir}/3rd-party-apis/labview],
                   [with_labview=no])])])

AS_CASE([$with_labview],
        [no], [],
        [yes], [AC_MSG_ERROR([Labview location required])],
        [LV_CINTOOLS="$with_labview/cintools"
         LV="mdsobjects/labview"])

AC_MSG_RESULT([$with_labview])

AC_MSG_CHECKING([whether to include idl support])
AC_ARG_WITH([idl],
            [AS_HELP_STRING([[--with-idl[=idl-top-dir]]],
                            [specify location of idl such as /usr/local/itt/idl, or set IDL_DIR])],
            [],
            [with_idl=yes])
AS_CASE([$with_idl],
        [yes], [AS_VAR_SET_IF([IDL_DIR],
                              [AS_VAR_SET([with_idl], [$IDL_DIR])],
                              [AS_IF([test -r /usr/local/itt/idl], [AS_VAR_SET([with_idl], [/usr/local/itt/idl])],
                                     [test -r /usr/local/rsi/idl], [AS_VAR_SET([with_idl], [/usr/local/rsi/idl])],
                                     [test -r /usr/local/exelis/idl], [AS_VAR_SET([with_idl], [/usr/local/exelis/idl])],
                                     [${srcdir}/conf/update_submodules && test -r ${srcdir}/3rd-party-apis/idl], [AS_VAR_SET([with_idl], [\${top_srcdir}/3rd-party-apis/idl])],
                                     [AS_VAR_SET([with_idl], [no])])])])
AS_CASE([$with_idl],
        [no], [AS_UNSET([IDL_DIR])
               AC_MSG_RESULT([$with_idl])],
        [AS_VAR_SET([IDL_INC], ["-I$with_idl/external"])
         AS_VAR_SET([IDLMDSEVENT], [idlmdsevent])
         AS_VAR_SET([IDLMDSWIDGETS], [idlmdswidgets])
         AC_MSG_RESULT([yes])])

AC_MSG_CHECKING([for mingw])
AS_CASE([$host],
        [*mingw*],[AS_VAR_SET([MINGW])])
AM_CONDITIONAL([MINGW], [AS_VAR_TEST_SET([MINGW])])
AM_COND_IF([MINGW], [AC_MSG_RESULT([yes])], [AC_MSG_RESULT([no])])

AC_MSG_CHECKING([for visual studio])
AC_ARG_WITH([visual_studio],
            [AS_HELP_STRING([--with-visual-studio],
                            [directory tree containing visual studio compiler, includes and libs])],
            [AS_VAR_SET([VS_DIR], [$with_visual_studio])],
            [AS_VAR_SET_IF([VS_DIR],
                           [AS_VAR_SET([with_visual_studio], [$VS_DIR])],
                           [AS_VAR_SET([with_visual_studio], [no])])])
AM_CONDITIONAL([VS],[AS_VAR_TEST_SET([VS_DIR])])
AC_MSG_RESULT([$with_visual_studio])

FORLD=ld
LD=ld LD_LDARC=""
LD_LDSHARE=""
IDL_LD=""
LIBPRE="lib"

CFLAGS="$CFLAGS $GCCPROF -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
CPPFLAGS="-I\${top_srcdir}/include -I\${top_builddir}/include $CPPFLAGS"

if test "$exec_prefix" = "NONE" -a "$prefix" = "NONE"
then
  : ${uiddir:=$ac_default_prefix/uid}
elif test "$exec_prefix" = "NONE"
then
  : ${uiddir:=$prefix/uid}
else
  : ${uiddir:=$exec_prefix/uid}
fi
if nproc&>/dev/null && perl -v&>/dev/null
then
  MAKEFLAG_J="-j -l $(perl -e "print $(nproc) * 0.9")"
else
  MAKEFLAG_J="-j -l 1"
fi
DEF_PYLIB=$(ldd $(which python) | grep libpython | awk '{print $3}')
MAKEFLAG_O="$(if ${MAKE:=make} -v -O&>/dev/null; then echo -O; fi)"
MAKEUIDDIR="\$(top_builddir)/uid/"
MAKESHLIBDIR="\$(top_builddir)/lib/"
MAKEBINDIR="\$(top_builddir)/bin/"
MAKELIBDIR="\$(top_builddir)/lib/"
EXPORTS_FILE=""
JDK_LIVECONNECT="$with_jdk/jre/lib/plugin.jar"
TAR_EXCLUDE="-e"
HUP_TO_XINETD=""
HUP_TO_INETD=""
UILCMD="uil"
FOR_LDFLAGS=""
SHARETYPE=".so"

THREAD="-pthread"
MITDEVICESIO_TARGETS=""
case "$host" in
*mingw*)
       MAKEFLAG_J=""
       MAKEFLAG_O=""
       IDLMDSEVENT=""
       LIBPRE=""
       CFLAGS="$CFLAGS -fno-strict-aliasing -mno-ms-bitfields ";
       LD="$CXX -static-libgcc"
       FCFLAGS="$FCFLAGS -fno-range-check"
       FORLD="$FC"
       if ( echo $host | grep i686 > /dev/null )
       then
         SQL_ARCH=32
         BINDIR=bin_x86
         VS_ARCH=x86
	 IDL_LIB="-L/mnt/scratch/mdsplus/idl-mingw-libs -lidl64"
       else
         SQL_ARCH=64
         BINDIR=bin_x86_64
         VS_ARCH=x64
	 IDL_LIB="-L/mnt/scratch/mdsplus/idl-mingw-libs -lidl32"
       fi
       MAKELIBDIR="\$(top_builddir)/${BINDIR}/"
       MAKESHLIBDIR="\$(top_builddir)/${BINDIR}/"
       MAKEBINDIR="\$(top_builddir)/${BINDIR}/"
       FCFLAGS="$FCFLAGS -fno-backslash";
       LDSHARE="-Wl,-Bdynamic ";
       LDARC="-Wl,-Bstatic ";
       LD_LDSHARE="-Bdynamic ";
       LD_LDARC="-Bstatic ";
       LINKSHARED="-shared ";
       FOR_LINKSHARED="$LINKSHARED";
       LIBPATH="LD_LIBRARY_PATH";
       FOR_LDFLAGS="";
       FEXECLIBDIR="-L";
       SHARETYPE=".dll"
       jni_inc_dir="$with_jdk/include";
       jni_md_inc_dir="$with_jdk/include/linux";;

*86*linux*)
       CFLAGS="$CFLAGS -fpic -shared-libgcc -fsigned-char -fno-strict-aliasing ";
       FORLD="$FC";
       LD="gcc"
       if ( echo $host| grep 64 > /dev/null )
       then
         if [[ -d /usr/lib64 ]]
         then
           D64="64"
         else
           D64=""
         fi
         SYB64=$D64
         TARGET_ARCH="-m64"
         UILCMD=uil
         uiddir="${uiddir}${D64}"
         MAKELIBDIR="\$(top_builddir)/lib${D64}/"
         MAKESHLIBDIR="\$(top_builddir)/lib${D64}/"
         MAKEUIDDIR="\$(top_builddir)/uid${D64}/"
	 MAKEBINDIR="\$(top_builddir)/bin${D64}/"
       else
         if [[ -d /usr/lib64 ]]
         then
           D32="32"
         else
           D32=""
         fi
         TARGET_ARCH="-m32"
         UILCMD=uil32
         uiddir="${uiddir}${D32}"
         MAKELIBDIR="\$(top_builddir)/lib${D32}/"
         MAKESHLIBDIR="\$(top_builddir)/lib${D32}/"
         MAKEUIDDIR="\$(top_builddir)/uid${D32}/"
         MAKEBINDIR="\$(top_builddir)/bin${D32}/"
       fi
       FCFLAGS="$FCFLAGS -fno-range-check"
       FOR_LDFLAGS="-lg2c"
       FCFLAGS="$FCFLAGS -fpic -fno-backslash";
       LDSHARE="-Wl,-Bdynamic ";
       LDARC="-Wl,-Bstatic ";
       LD_LDSHARE="-Bdynamic ";
       LD_LDARC="-Bstatic ";
       LINKSHARED="-shared ";
       FOR_LINKSHARED="$LINKSHARED";
       LIBPATH="LD_LIBRARY_PATH";
       FOR_LDFLAGS="";
       FEXECLIBDIR="-L";
       TAR_EXCLUDE="--exclude";
       MITDEVICESIO_TARGETS="${MAKESHLIBDIR}libMitDevicesIO${SHARETYPE} ${MAKELIBDIR}libMitDevicesIO.a ${MAKEBINDIR}daq_server"; 
       jni_inc_dir="$with_jdk/include";
       jni_md_inc_dir="$with_jdk/include/linux";
       HUP_TO_XINETD="/etc/rc.d/init.d/xinetd restart";
       HUP_TO_INETD="kill -HUP \`/sbin/pidof inetd\`";
       ;;
*arm-xilinx-linux-gnueabi) echo "configuring for Xylinx zynq board (arm cortex a9)";
       CC=arm-xilinx-linux-gnueabi-gcc;
       LD=arm-xilinx-linux-gnueabi-ld;
       AR=arm-xilinx-linux-gnueabi-ar;
       RANLIB=arm-xilinx-linux-gnueabi-ranlib;
       SIZEOF_LONG=4;
       SIZEOF_LONG_LONG=8;
       D64="";
       CFLAGS="$CFLAGS -fpic -shared-libgcc -fsigned-char -fno-strict-aliasing";
       if (test "$FC" = "gfortran"); then
         FCFLAGS="$FCFLAGS -fno-range-check";
         FORLD="$FC"
         FOR_LDFLAGS="-lg2c"
       fi
       FCFLAGS="$FCFLAGS -fpic -fno-backslash";
       LDSHARE="-Wl,-Bdynamic ";
       LDARC="-Wl,-Bstatic ";
       LD_LDSHARE="-Bdynamic ";
       LD_LDARC="-Bstatic ";
       LINKSHARED="-shared ";
       FOR_LINKSHARED="$LINKSHARED";
       LIBPATH="LD_LIBRARY_PATH";
       FOR_LDFLAGS="";
       FEXECLIBDIR="-L";
       TAR_EXCLUDE="--exclude";
       jni_inc_dir="$with_jdk/include";
       jni_md_inc_dir="$with_jdk/include/linux";
       HUP_TO_XINETD="/etc/rc.d/init.d/xinetd restart";
       HUP_TO_INETD="kill -HUP \`/sbin/pidof inetd\`";;
*linux*) CFLAGS="$CFLAGS -fpic -fsigned-char -shared-libgcc -fno-strict-aliasing";
       FCFLAGS="$FCFLAGS -fno-range-check";
       FORLD="$FC"
       FOR_LDFLAGS="-lg2c"
       FCFLAGS="$FCFLAGS -fpic -fno-backslash";
       LDSHARE="-Wl,-Bdynamic ";
       LDARC="-Wl,-Bstatic ";
       LD_LDSHARE="-Bdynamic ";
       LD_LDARC="-Bstatic ";
       LINKSHARED="-shared ";
       FOR_LINKSHARED="$LINKSHARED";
       LIBPATH="LD_LIBRARY_PATH";
       FOR_LDFLAGS="";
       FEXECLIBDIR="-L";
       TAR_EXCLUDE="--exclude";
       jni_inc_dir="$with_jdk/include";
       jni_md_inc_dir="$with_jdk/include/linux";
       HUP_TO_XINETD="/etc/rc.d/init.d/xinetd restart";
       HUP_TO_INETD="kill -HUP \`/sbin/pidof inetd\`";;
*apple-darwin*) echo "Configuring for MacOS X";
       THREAD=""
       MACOSX="macosx"
       CFLAGS="$CFLAGS -arch x86_64 -dynamic -Wno-error=unused-command-line-argument -Wno-missing-field-initializers -Wno-parentheses-equality";
       FCFLAGS="$FCFLAGS -fno-range-check"
       FORLD="gfortran"
       FOR_LDFLAGS="-shared"
       FEXECLIBDIR="-L";
       LD="$CC"
       LDSHARE="";
       LD_LDSHARE="";
       LDARC="";
       LD_LDARC="";
       UILPATH=${OPENMOTIF-/usr}/bin;
       MOTIF_LDARC="-Wl,-bind_at_load -multiply_defined suppress -L${OPENMOTIF-/usr}/lib"
       MOTIF_LD_LDARC="-multiply_defined suppress -L${OPENMOTIF-/usr}/lib"
       dnl LINKSHARED="$LDFLAGS -dynamiclib -install_name $libdir/\$(@F) -headerpad_max_install_names -prebind \
	   dnl -seg_addr_table_filename \$(@F) -seg_addr_table ../macosx/bindtable -Wl,-single_module";
	   LINKSHARED="$LDFLAGS -shared -arch x86_64 -install_name @loader_path/../lib/\$(@F) -headerpad_max_install_names";
           FOR_LINKSHARED="$LDFLAGS -shared";
	   LINKMODULE="$LDFLAGS -bundle -undefined dynamic_lookup";
       IDL_LD="";
       LINKJNI="$LINKSHARED";
       SHARETYPE=".dylib";
       SHARETYPEJNI=".dylib";
       SHARETYPEMOD=".dylib";
       TAR_EXCLUDE="--exclude";
       JDK_DIR=/Library/Java/Home
       jni_inc_dir="/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers";
       jni_md_inc_dir="$jni_inc_dir";
       JDK_LIVECONNECT="$JDK_DIR/lib/plugin.jar"
       java_enable="yes"
       LIBPATH="DYLD_LIBRARY_PATH"
       dnl HDF5_LIBS="-lz -lsz";
       dnl HDF5_DIR="/usr/local";
       X_EXTRA_LIBS="-lXmu";;
esac


AC_CHECK_SIZEOF(long)

# Stuff that only non-automake dirs need
AC_SUBST([MAKEETCDIR], ["\$(top_builddir)/etc/"])

dnl Check for 0xnnnnll (long long constants)
AC_LANG_SAVE
AC_LANG_C
AC_TRY_COMPILE(,int gub(){return((int)0x1ll);}
 ,AC_DEFINE(HAVE_LL_CONSTANTS
  ,,"Define if you compiler likes 0xnnnnll long long constants"))
AC_LANG_RESTORE

AC_MSG_CHECKING(if JNI libraries are different)
if test "$LINKJNI"
then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
	SHARETYPEJNI="$SHARETYPE"
        LINKJNI="$LINKSHARED"
fi

AC_MSG_CHECKING(if IDL plugins are different)
if test "$LINKMODULE"
then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
	SHARETYPEMOD="$SHARETYPE"
        LINKMODULE="$LINKSHARED"
fi


dnl Checks for libraries.

AC_DEFUN([MDS_SEARCH_LIBS],[
  old_LIBS="$LIBS"
  AC_SEARCH_LIBS([$1],[$2],[$3],[$4],[$5])
  LIBS="$old_LIBS"
  ])

#
# Add TARGET_ARCH to CFLAGS so that all library searches will look for libraries of the correct arch
#
CFLAGS_lib_check_save="$CFLAGS"
AS_VAR_SET_IF([TARGET_ARCH],[CFLAGS="$CFLAGS $TARGET_ARCH"])
#

dnl see if we have libdc1394 libraries and what version

raw_old_libs="$LIBS" #### needed because of nested call to MDS_SEARCH_LIBS
MDS_SEARCH_LIBS([raw1394_get_libversion],[raw1394],
[
  MDS_SEARCH_LIBS([dc1394_new],[dc1394],[DC1394_SUPPORT2="${MAKESHLIBDIR}libdc1394_support2$SHARETYPE"])
  MDS_SEARCH_LIBS([dc1394_get_camera_info],[dc1394],[DC1394_SUPPORT="${MAKESHLIBDIR}libdc1394_support$SHARETYPE"])
])
LIBS="$raw_old_libs"

AC_SEARCH_LIBS([gethostbyname], [nsl socket],
               [AS_VAR_IF([ac_cv_search_gethostbyname], ["none required"], [], [LIBSOCKET=$ac_cv_search_gethostbyname])],
               [dnl Can't search ws2_32 for gethostbyname using AC_SEARCH_LIBS, because
                dnl it requires #include <winsocks2.h> to work.
                AC_MSG_CHECKING([for gethostbyname in ws2_32])
                LIBSOCKET="-lws2_32"
                mds_old_LIBS="$LIBS"
                LIBS="$LIBSOCKET $LIBS"
                AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <winsock2.h>]],
                                                [[gethostbyname ("");]])],
                               [AC_MSG_RESULT([yes])],
                               [AC_MSG_RESULT([no]); AS_UNSET([LIBSOCKET])])
                LIBS=$mds_old_LIBS])
AC_SUBST([LIBSOCKET])

MDS_SEARCH_LIBS([pow],[m],[AS_VAR_IF([ac_cv_search_pow],["none required"],[],[AS_VAR_SET([LIBM],[$ac_cv_search_pow])])])
MDS_SEARCH_LIBS([__dn_skipname],[resolv],[AS_VAR_IF([ac_cv_search___dn_skipname],["none required"],[],[AS_VAR_SET([LIBRESOLV],[$ac_cv_search___dn_skipname])])])
MDS_SEARCH_LIBS([dlopen],[dl],[AS_VAR_IF([ac_cv_search_dlopen],["none required"],[],[AS_VAR_SET([LIBDL],[$ac_cv_search_dlopen])])])
MDS_SEARCH_LIBS([getgrgid],[],[AC_DEFINE([HAVE_GETGRGID],[],["Define if you have getgrgid to get group name."])])
MDS_SEARCH_LIBS([getpwuid],[],[AC_DEFINE([HAVE_GETPWUID],[],["Define if you have getpwuid."])])
MDS_SEARCH_LIBS([gethostbyname],[dnet_stub],[AS_VAR_IF([ac_cv_search_gethostbyname],["none required"],[],[AS_VAR_SET([DNET_STUB],[$ac_cv_search_gethostbyname])])])
MDS_SEARCH_LIBS([clock_gettime],[rt],
  [
    AC_DEFINE([HAVE_CLOCK_GETTIME],[],["Define if you have the clock_gettime function."])
    AS_VAR_IF([ac_cv_search_clock_gettime],["none required"],[],[AS_VAR_SET([CLOCK_GETTIME_LIB],["$ac_cv_search_clock_gettime"])])
  ])
MDS_SEARCH_LIBS([gettimeofday],[],[AC_DEFINE([HAVE_GETTIMEOFDAY],[],["Define if you have the gettimeofday function."])])
MDS_SEARCH_LIBS([getaddrinfo],[],[AC_DEFINE([HAVE_GETADDRINFO],[],["Define if you have the getaddrinfo routine"])])
MDS_SEARCH_LIBS([strsep],[],[AC_DEFINE([HAVE_GETADDRINFO],[],["Define if you have the getaddrinfo routine"])])
MDS_SEARCH_LIBS([getrusage],[],[AC_DEFINE(HAVE_GETRUSAGE,,"Define if you have the getrusage routine")])

AC_ARG_ENABLE(d3d,
	[  --enable-d3d            build d3d ptdata access library ],
	if test "$enableval" = yes; then
          AC_MSG_CHECKING(...D3DLIB_PATH...)
          : ${D3DLIB_PATH:?"You must define D3DLIB_PATH to be the directory containing the libd3 library"}
          if test -r "$D3DLIB_PATH/libd3.a" -o -r "$D3DLIB_PATH/libd3$SHARETYPE"
          then
		 D3D_PACKAGE="\$(D3D_PACKAGE)";
		 D3DLIB="-L$D3DLIB_PATH";
                 AC_MSG_RESULT($D3DLIB)
          else
		 AC_MSG_RESULT(libd3.a or libd3$SHARETYPE not found in $D3DLIB_PATH)
                 exit 0
          fi
        fi)



## ////////////////////////////////////////////////////////////////////////// ##
## //  READLINE  //////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##


AC_SEARCH_READLINE([READLINE])
AM_CONDITIONAL(READLINE, test x"$have_readline" = x"yes")

dnl sets for retro compatibility
if test x"$have_readline" = x"yes"; then
  LIBREADLINE="${READLINE_LIBS}"
  TDIC="tdic"

  dnl  CPPFLAGS_save="${CPPFLAGS}"
  CPPFLAGS="${READLINE_CPPFLAGS} ${CPPFLAGS}"
  LDFLAGS="${READLINE_LDFLAGS} ${LDFLAGS}"
  AC_CHECK_HEADERS(readline/readline.h readline/history.h
     ,,AC_MSG_ERROR("libreadline headers was not found"))
  dnl this appears to be used so I add it here also.
  AC_CHECK_FUNCS(rl_set_signals)
  dnl  CPPFLAGS=$CPPFLAGS_save
  dnl else
  dnl  AC_MSG_ERROR(readline library was not found and must be installed.)
fi

dnl //// discontinued implementation ////
dnl LIBS_save="$LIBS"
dnl   AC_SEARCH_LIBS([readline],[readline 'readline -lcurses'])
dnl LIBS="$LIBS_save"
dnl if test "$ac_cv_search_readline" = "no"
dnl then
dnl   AC_MSG_RESULT("libreadline is not available so the build of tdic will be skipped")
dnl   TDIC=""
dnl   LIBREADLINE=""
dnl else
dnl   LIBREADLINE="$ac_cv_search_readline"
dnl   TDIC="tdic"
dnl fi
dnl AC_SEARCH_LIBS([rl_set_signals],[readline 'readline -lcurses'],AC_DEFINE(HAVE_RL_SET_SIGNALS,,"define variable in code"))
dnl AC_CHECK_HEADERS(readline/readline.h readline/history.h)


## ////////////////////////////////////////////////////////////////////////// ##
## //  LIBXML  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

AM_PATH_XML2('2.0.0', [have_xml=yes], [have_xml=no])
if test "x$have_xml" != "xyes"; then
 AC_MSG_NOTICE([libxml not found using xml2-config, reverting to pkg-config query])
 PKG_CHECK_MODULES([XML], [libxml-2.0], [have_xml=yes], [have_xml=no])
 XML_CPPFLAGS="$XML_CFLAGS"
fi
AC_SUBST(XML_CFLAGS)
AC_SUBST(XML_CPPFLAGS)
AC_SUBST(XML_LIBS)
AM_CONDITIONAL(XML, test x"$have_xml" = x"yes")

if test "x$have_xml" = "xyes"; then
 dnl test also for headers usability required by mdsdcl
 CPPFLAGS_save="$CPPFLAGS"
 CPPFLAGS="$XML_CPPFLAGS $CPPFLAGS $TARGET_ARCH"
 AC_CHECK_HEADERS(libxml/tree.h libxml/parser.h libxml/xpath.h libxml/xpathInternals.h
  ,,AC_MSG_ERROR(libxml2 development files was not found and must be installed.))
 CPPFLAGS="$CPPFLAGS_save"
else
 AC_MSG_ERROR(libxml2 was not found and must be installed.)
fi


## ////////////////////////////////////////////////////////////////////////// ##
## //  HEADERS  ///////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

dnl Checks for header files.
AC_CHECK_HEADERS(stdarg.h fcntl.h strings.h sys/ioctl.h syslog.h unistd.h sys/filio.h netdb.h resolv.h sys/types.h linux/types.h drm/drm.h pwd.h grp.h)
AC_CHECK_HEADERS(dlfcn.h dl.h vxWorks.h windows.h sys/resource.h)
AC_CHECK_HEADERS(malloc.h alloca.h)


## PTHREADS ##
AC_CHECK_HEADERS(pthread.h)
AC_CHECK_FUNCS(pthread_lock_global_np)






REMCAM="remcam"
AC_CHECK_HEADERS(scsi/sg.h,DO_CAMSHR="yes",DO_CAMSHR="no")
if test "$DO_CAMSHR" = yes
then
  CAMSHR="camshr"
  REMCAMLIB="RemCamShr"
else
  CAMSHR=""
  REMCAMLIB="CamShr"
fi

AC_CHECK_HEADERS([stdint.h])

dnl Check for user specified  hdf5 header and library
if test -n "$HDF5_DIR" -a -r "$HDF5_DIR"
then
      HDF5_APS="\$(HDF5_APS)"
      HDF5_INCS="-I$HDF5_DIR/include"
      HDF5_LIBS="$HDF5_LIBS -L$HDF5_DIR/lib"
else

dnl Check for default hdf5 header and library
HDF5_APS=""
AC_CHECK_HEADERS(hdf5.h,
[
  MDS_SEARCH_LIBS([H5Fopen],[hdf5],
  [
    HDF5_APS="\$(HDF5_APS)"
    HDF5_INCS=""
    HDF5_LIBS=""
  ])
])

fi

dnl Check for jdk files
OLD_CPPFLAGS=$CPPFLAGS
AS_VAR_SET_IF([JNI_INCLUDE_DIR],[jni_inc_dir="$JNI_INCLUDE_DIR"])
AS_VAR_SET_IF([JNI_MD_INCLUDE_DIR],[jni_md_inc_dir="$JNI_MD_INCLUDE_DIR"])
CPPFLAGS="$CPPFLAGS -I$jni_inc_dir -I$jni_md_inc_dir"
AC_CHECK_HEADER(jni.h,JDK_CFLAGS="-I$jni_inc_dir")
AC_CHECK_HEADER(jni_md.h,JDK_CFLAGS="$JDK_CFLAGS -I$jni_md_inc_dir")
AS_VAR_SET_IF([JDK_CFLAGS], [AS_CASE([$enable_java], [yes], [JAVA_APS="\$(JAVA_APS)";JAVA_JAR="\$(JAVA_JAR)"])])
CPPFLAGS="$OLD_CPPFLAGS"

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T

dnl Check if we want to try Named Pipes instead of <sys/msg.h> API
AC_CHECK_HEADERS(sys/msg.h,mdsplus_ok=no,mdsplus_ok=yes)
if test "$mdsplus_ok" = yes; then
   AC_CHECK_HEADERS(sys/stat.h, mdsplus_ok=yes, mdsplus_ok=no)
   AC_MSG_CHECKING(if we should use pipes for messaging)
   if test "$mdsplus_ok" = yes; then
        AC_DEFINE(USE_PIPED_MESSAGING,,"Define if use piped messaging")
        AC_MSG_RESULT($mdsplus_ok)
    fi
fi

#AC_CACHE_CHECK([for timezone and daylight externals], mdsplus_cv_have_timezone,
#    AC_TRY_LINK([#include <time.h>], [long z2 = timezone - daylight*3600;],
#        mdsplus_cv_have_timezone=yes, mdsplus_cv_have_timezone=no))
#    if test "$mdsplus_cv_have_timezone" = yes; then
#        AC_DEFINE(HAVE_TIMEZONE,[],["define HAVE_TIMEZONE"])
#    else
#        AC_CACHE_CHECK([for tm_gmtoff in struct tm], mdsplus_cv_tm_gmtoff_in_tm,
#            AC_TRY_LINK([#include <time.h>], [struct tm tim; tim.tm_gmtoff = 0;],
#            mdsplus_cv_tm_gmtoff_in_tm=yes,mdsplus_cv_tm_gmtoff_in_tm=no))
#    fi
#    if test "$mdsplus_cv_tm_gmtoff_in_tm" = yes; then
#        AC_DEFINE(USE_TM_GMTOFF,[],["define USE_TM_GMTOFF"])
#    fi
AC_CACHE_CHECK([for tm_gmtoff in struct tm], mdsplus_cv_tm_gmtoff_in_tm,
     AC_TRY_LINK([#include <time.h>], [struct tm tim; tim.tm_gmtoff = 0;],
         mdsplus_cv_tm_gmtoff_in_tm=yes,mdsplus_cv_tm_gmtoff_in_tm=no))
     if test "$mdsplus_cv_tm_gmtoff_in_tm" = yes; then
        AC_DEFINE(USE_TM_GMTOFF
         ,,"Define if use need to use localtime instead of timezone and daylight")
     fi

AC_MSG_CHECKING(for semun union declaration)
AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/sem.h>],[union semun arg;],
							 mdsplus_ok=yes,mdsplus_ok=no)
if test $mdsplus_ok = no; then
		 AC_DEFINE(NEED_SEMUN,,"Define NEED_SEMUN")
fi
AC_MSG_RESULT($mdsplus_ok)

AC_MSG_CHECKING(for two byte fd in *FILE)
AC_EGREP_HEADER(_fileL,stdio.h,mdsplus_ok=yes,mdsplus_ok=no)
if test $mdsplus_ok = yes; then
		 AC_DEFINE(FILE_PTR_HL,,"define FILE_PTR_HL")
fi
AC_MSG_RESULT($mdsplus_ok)

dnl Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_HEADER_STDC
AC_CHECK_FUNCS(gethostname gettimeofday select socket)
AC_CHECK_FUNC(nis_local_host,mdsplus_ok=yes,mdsplus_ok=no)
AC_CHECK_FUNC(malloc)
AC_CHECK_FUNC(sched_setscheduler)

dnl Check for Sybase
if test "$host_cpu" = "arm" -a "$(echo $host | grep arm-xilinx)" != ""
then
   SYBASE_INC=""
   SYBASE_LIB=""
   SYBASE=""
   AC_MSG_RESULT(no sybase open/client)
elif ( echo $host | grep "mingw" >/dev/null )
then
    SYBASE_INC="-DSYBASE"
    SYBASE_LIB="-L\${srcdir} -lntwdblib${SQL_ARCH}"
    SYBASE="SYBASE"
    AC_MSG_RESULT(using Windows mssql)
elif test "$(echo $SYBASE_LIB | grep '\-L')" = ""
then
    SYBASE_INC=""
    SYBASE_LIB=""
    OLDLIBS="$LIBS"
    AC_SEARCH_LIBS([dbsqlexec],[sybdb],[SYBASE_LIB="-lsybdb";SYBASE_INC="-DSYBASE";SYBASE="SYBASE",SYBASE_LIB=""],)
    LIBS="$OLDLIBS"
    if test "$SYBASE_LIB" = ""
    then
      AC_MSG_CHECKING(for SYBASE open/client or freetds)
      if test -r "/usr/lib${SYB64}/libsybdb.so"
      then
          SYBASE_INC="-DSYBASE"
          SYBASE_LIB="-lsybdb"
          SYBASE="SYBASE"
          AC_MSG_RESULT(have freetds)
      else
        : ${SYBASE=/usr/local/sybase}
        if test -r "$SYBASE/lib"
	then SYBASE_LIBDIR="$SYBASE/lib"
	elif test -r "$SYBASE/lib${SYB64}"
	then SYBASE_LIBDIR="$SYBASE/lib${SYB64}"
	fi
	if test "$SYBASE_LIBDIR" != ""
	then
	  if test -r "$SYBASE_LIBDIR/libsybdb.a"
	  then
	    SYBASE_LIB="-L$SYBASE_LIBDIR $LD_LDARC -lsybdb $LD_LDSHARE"
	  else
	    SYBASE_LIB="-L$SYBASE_LIBDIR -lsybdb"
	  fi
	  SYBASE_INC="-I$SYBASE/include -DSYBASE"
	  AC_MSG_RESULT(have sybase)
	else
	  SYBASE=""
          AC_MSG_RESULT(no sybase open/client)
	fi
      fi
    fi
else
  SYBASE="SYBASE"
fi
AC_MSG_CHECKING(if mdssql should be built)
if test -n "$SYBASE_INC"
then
        MDSSQL="mdssql"
        AC_MSG_RESULT(have sql so yes.)
        AC_DEFINE(HAVE_SYBASE,,"Define if you have the sybase includes (freetds)")
else
        MDSSQL=""
        AC_MSG_RESULT(missing sql so no.)
fi

AC_DEFINE_UNQUOTED(SHARELIB_TYPE,"${SHARETYPE}",
                   [Set SHARELIB_TYPE to ".so",".sl",".a" etc...])
AC_C_BIGENDIAN

AC_PATH_XTRA
AS_CASE([$host],
        [*darwin*|*apple*], [X_CFLAGS+=" -I${OPENMOTIF-/usr}/include"
                             X_LIBS+=" -L${OPENMOTIF-/usr}/lib"])
AS_CASE([$host],[arm*gnueabihf],[X_CFLAGS=""])
#FIXME: Change the above to AS_VAR_APPEND with a new autoconf version (2.64+?)

if test $UILCMD = uil32
then
  X_LIBS=`echo $X_LIBS | sed s/lib64/lib/`
fi
AC_PATH_PROG(UIL,$UILCMD,$UILCMD,$UILPATH:/usr/bin/X11:$PATH:/usr/dt/bin)
AC_MSG_CHECKING(for uil command)
if test ! "$MINGW" = "yes" && ( which $UIL &>/dev/null )
then
  UIL="env LANG=en_US $UIL"
  HAVE_MOTIF="yes"
  #AC_MSG_RESULT(have uil so openmotif apps will be built.)
else
# IDLMDSEVENT and IDLMDSWIDGETS need MOTIF to build
  IDLMDSEVENT=""
  IDLMDSWIDGETS=""
  #AC_MSG_RESULT(no uil so skipping openmotif apps.openmotif-devel not installed.)
fi
MOTIF_APS="\$(MOTIF_APS)"
AM_CONDITIONAL([HAVE_MOTIF], [AS_VAR_TEST_SET([HAVE_MOTIF])])
AM_COND_IF([HAVE_MOTIF], [AC_MSG_RESULT([yes])], [AC_MSG_RESULT([no])])

XM_LIBS="-lMrm -lXm"
MDS_SEARCH_LIBS([XextAddDisplay],[Xext],[AS_VAR_SET([LIBXEXT],[$ac_cv_search_XextAddDisplay])])
MDS_SEARCH_LIBS([XpGetDocumentData],[Xp],[AS_VAR_SET([LIBXP],[$ac_cv_search_XpGetDocumentData])])

## ////////////////////////////////////////////////////////////////////////// ##
## //  PYTHON   ///////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

## TODO: Add version here

dnl Find a Python2 interpreter.  Python versions prior to 2.0 are not
dnl supported. (2.0 was released on October 16, 2000).
m4_define([_AM_PYTHON_INTERPRETER_LIST],
	  [python2.7 python2.6 python2.5 python2.4 python2.3 python2.2 python2.1 python2.0 python2])
AM_PATH_PYTHON([2],,[:])
AM_CONDITIONAL([HAVE_PYTHON], [test "$PYTHON" != :])

AX_PYTHON_ARCH(PYTHON_ARCHITECTURE)
AC_SUBST(PYTHON_ARCHITECTURE)


## ////////////////////////////////////////////////////////////////////////// ##
## //  TESTS    ///////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

## MdsTestShr CHECK backend related ##
AC_CHECK_FUNCS(mkstemp)
AC_CHECK_FUNCS(fork)
### maybe needs to be changed. Not sure how to deal with the multiple routines and ac_cv_search_xxxx
AC_CHECK_LIB([rt],  [clock_gettime, timer_create, timer_settime, timer_delete],
             [LIBRT="-lrt"],[LIBRT=""])


# check that struct timespec is defined in time.h. If not, we need to
# define it in libcompat.h. Note the optional inclusion of pthread.h.
# On MinGW and MinGW-w64, the pthread.h file contains the timespec
# definition.
AC_CHECK_MEMBERS([struct timespec.tv_sec, struct timespec.tv_nsec], [], [AC_DEFINE_UNQUOTED(STRUCT_TIMESPEC_DEFINITION_MISSING, 1, "Need to define the timespec structure")], [
#include <time.h>
#if defined(HAVE_PTHREAD)
#include <pthread.h>
#endif /* HAVE_PTHREAD */
])

# check that struct itimerspec is defined in time.h. If not, we need to
# define it in libcompat.h.  Note the optional inclusion of pthread.h.
# On MinGW and MinGW-w64, the pthread.h file contains the itimerspec
# definition.
AC_CHECK_MEMBERS([struct itimerspec.it_interval, struct itimerspec.it_value], [], [AC_DEFINE_UNQUOTED(STRUCT_ITIMERSPEC_DEFINITION_MISSING, 1, "Need to define the itimerspec structure")], [
#include <time.h>
#if defined(HAVE_PTHREAD)
#include <pthread.h>
#endif /* HAVE_PTHREAD */
])


AC_CHECK_PROG(HAVE_WINE, wine, yes,no)
AM_CONDITIONAL(HAVE_WINE, test x"${HAVE_WINE}" = x"yes")

AC_CHECK_PROG(HAVE_WINECONSOLE, wineconsole, yes,no)
AM_CONDITIONAL(HAVE_WINECONSOLE, test x"${HAVE_WINECONSOLE}" = x"yes")

AX_PERL_MODULE_VERSION(TAP::Parser 0 TAP::Parser::Aggregator 0,
     [AS_VAR_SET([have_pl_tap_parser],[yes])],
     [AS_VAR_SET([have_pl_tap_parser],[no])
      AC_MSG_WARN([No Perl tap module found])
     ])
AM_CONDITIONAL([HAVE_PL_TAP_PARSER],[test x"${have_pl_tap_parser}" = x"yes"])

 # check for valgrind
 AX_VALGRIND_CHECK([ENABLE_VALGRIND])

 # check for sanitizer
 AX_ENABLE_SANITIZE([ENABLE_SANITIZE],
                    [AS_VAR_SET([CPPFLAGS],["${sanitize_flags} $CPPFLAGS"])
                     AS_VAR_SET([LIBS],["${sanitize_libs} $LIBS"])],
                    [echo "sanitizer not available."; exit 111;])


 # selects particular envs for build:host
 TS_SELECT

 ## test chain substitution ##
 AC_SUBST(TESTS_ENVIRONMENT)
 AC_SUBST(LOG_COMPILER)
 AC_SUBST(LOG_DRIVER)
 AC_SUBST(PY_LOG_COMPILER)
 AC_SUBST(PY_LOG_FLAGS)

 AC_SUBST(VALGRIND_FLAGS)
 AC_SUBST(VALGRIND_memcheck_FLAGS)


## ////////////////////////////////////////////////////////////////////////// ##
## //  DOXYGEN  ///////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

## minimum requred doxygen for cute documentation rendering
DOXYGEN_VERSION_REQUIRED="1.8"

## versioning check script for sh usign sort builtin command
## check if $2 is a version higher than $1
function _ver_higher {
        ver=`echo -ne "$1\n$2" |sort -Vr |head -n1`
        if [[ "$2" == "$1" ]]; then return 1
        elif [[ "$2" == "$ver" ]]; then return 0
        else return 1
        fi
}

## checking for doxygen bin in path (see also m4 macro below)
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
  then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
  AM_CONDITIONAL([HAVE_CUTE_DOXYGEN], [false])
else
  AM_CONDITIONAL([HAVE_CUTE_DOXYGEN],
    [_ver_higher $DOXYGEN_VERSION_REQUIRED `$DOXYGEN --version`])
  AM_COND_IF([HAVE_CUTE_DOXYGEN],
    [ echo "Doxygen version is cute!" ],
    [ AC_MSG_WARN(["Doxygen version outdated, very limited documentation support"]) ])
fi

## DEFAULT ACTIVE FEATURES
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(OFF)

## refers to ax_prog_doxygen.m4 macro expansion
DX_INIT_DOXYGEN(mdsplus, doxygen.cfg)

#
# All library searches should be done at this point so remove first
# TARGET_ARCH from CFLAGS as the Make macros will insert TARGET_ARCH later
# as needed.
#
CFLAGS="$CFLAGS_lib_check_save"
#

# //////////////////////////////////////////////////////////////////////////// #
# ////// RELEASE INFO //////////////////////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #

if [[ ! -z ${RELEASE_VERSION} ]]
then
  RELEASE_MAJOR=$(echo ${RELEASE_VERSION} | cut -d. -f1)
  RELEASE_MINOR=$(echo ${RELEASE_VERSION} | cut -d. -f2)
  RELEASE_RELEASE=$(echo ${RELEASE_VERSION} | cut -d. -f3)
  RELEASE_BRANCH=$BRANCH
  RELEASE_COMMIT=$GIT_COMMIT
else
  RELEASE_MAJOR=1
  RELEASE_MINOR=0
  RELEASE_RELEASE=0
  RELEASE_BRANCH="unknown"
  RELEASE_COMMIT="unknown"
fi
RELEASE_DATE="$(date)"
RELEASE_TAG="${BRANCH}_release-${RELEASE_MAJOR}-${RELEASE_MINOR}-${RELEASE_RELEASE}"

# Determine common warning options to apply to C, C++, and Fortran languages
AS_VAR_SET([WARNFLAGS], ["-Wall -Wextra"])
AS_VAR_SET([NOWARNFLAGS],[])
AC_MSG_CHECKING([whether to treat warnings as errors])
AC_ARG_ENABLE([werror],
              [AS_HELP_STRING([--enable-werror],
                              [Treat all warnings as errors])],
              [],
              [AS_VAR_SET([enable_werror], [no])])
AS_CASE([$enable_werror],
        [yes], [AS_VAR_APPEND([WARNFLAGS], [" -Werror "])],
        [])
AC_ARG_ENABLE([wreturns],
              [AS_HELP_STRING([--disable-wreturns],
                              [Disable warnings about unused return value from functions])],
              [AS_CASE([$enableval],[no], [AS_VAR_APPEND([NOWARNFLAGS],[" -Wno-unused-result "])])],
	      [])
AC_SUBST([WARNFLAGS])
AC_SUBST([NOWARNFLAGS])
AC_MSG_RESULT([$enable_werror])

AC_MSG_CHECKING([whether to warn about implicit conversions])
AC_ARG_ENABLE([wconversion],
              [AS_HELP_STRING([--enable-wconversion],
                              [Warn about implicit conversions])],
              [],
              [AS_VAR_SET([enable_wconversion], [no])])
AS_CASE([$enable_wconversion],
        [yes], [AS_VAR_APPEND([WARNFLAGS], [" -Wconversion"])],
        [])
AC_SUBST([WARNFLAGS])
AC_MSG_RESULT([$enable_wconversion])


# //////////////////////////////////////////////////////////////////////////// #
# ///  CONFIG_H  ///////////////////////////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #

dnl Here we define all custom autoheader adds to mdsconfig.h.in

dnl add here what goes on tom of mdsconfig.h
AH_TOP()

AH_VERBATIM([_LL_CONSTANTS], [
#ifdef HAVE_LL_CONSTANTS
#define LONG_LONG_CONSTANT(value) value##ll
#else
#define LONG_LONG_CONSTANT(value) value
#endif
])


AH_VERBATIM([_WIN32], [
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#elif HAVE_VISIBILITY
# define EXPORT __attribute__((__visibility__("default")))
#else
#define EXPORT
#endif
])

dnl add here what goes on bottom of mdsconfig.h
AH_BOTTOM()


dnl define custom direcotries substitution
AX_MDSPLUS_DIRECTORIES

AC_SUBST(CAMSHR)
AC_SUBST(CLOCK_GETTIME_LIB)
AC_SUBST(D3D_PACKAGE)
AC_SUBST(D3DLIB)
AC_SUBST(D3D_FFLAGS)
AC_SUBST(DNET_STUB)
AC_SUBST(EXPORTS_FILE)
AC_SUBST(FEXECLIBDIR)
AC_SUBST(FORLD)
AC_SUBST(FOR_LDFLAGS)
AC_SUBST(FOR_LINKSHARED)
AC_SUBST(GEN_DEVICE)
AC_SUBST(GLOBUS_CFLAGS)
AC_SUBST(GLOBUS_FLAVOR)
AC_SUBST(GLOBUS_LIBS)
AC_SUBST(GLOBUS_LOCATION)
AC_SUBST(HDF5_INCS)
AC_SUBST(HDF5_LIBS)
AC_SUBST(HDF5_APS)
AC_SUBST(HUP_TO_INETD)
AC_SUBST(HUP_TO_XINETD)
AC_SUBST(IDL_INC)
AC_SUBST(IDL_LIB)
AC_SUBST(IDLMDSEVENT)
AC_SUBST(IDLMDSWIDGETS)
AC_SUBST(JARS_DIR)
AC_SUBST(JAVA_ONLY)
AC_SUBST(JAVA_APS)
AC_SUBST(JAVA_JAR)
AC_SUBST(JAVACFLAGS)
AC_SUBST(JDK_CFLAGS)
AC_SUBST(JDK_DIR)
AC_SUBST(JDK_LIVECONNECT)
AC_SUBST(LD)
AC_SUBST(LDSHARE)
AC_SUBST(LDARC)
AC_SUBST(LD_LDSHARE)
AC_SUBST(LD_LDARC)
AC_SUBST(LIBDL)
AC_SUBST(LIBM)
AC_SUBST(LIBPATH)
AC_SUBST(LIBPRE)
AC_SUBST(LIBREADLINE)
AC_SUBST(LIBRESOLV)
AC_SUBST(LIBXP)
AC_SUBST(LIBXEXT)
AC_SUBST(LIBRT)
AC_SUBST(LINKSHARED)
AC_SUBST(LINKJNI)
AC_SUBST(LINKMODULE)
AC_SUBST(LV)
AC_SUBST(LV_CINTOOLS)
AC_SUBST(MACOSX)
AC_SUBST(MAKEBINDIR)
AC_SUBST(MAKEFLAG_J)
AC_SUBST(MAKEFLAG_O)
AC_SUBST(MAKELIBDIR)
AC_SUBST(MAKESHLIBDIR)
AC_SUBST(MAKEUIDDIR)
AC_SUBST(MDSSQL)
AC_SUBST(MDSTCPIP)
AC_SUBST(MITDEVICESIO_TARGETS)
AC_SUBST(HAVE_MOTIF)
AC_SUBST(MOTIF_APS)
AC_SUBST(MOTIF_LDSHARE)
AC_SUBST(MOTIF_LDARC)
AC_SUBST(MOTIF_LD_LDSHARE)
AC_SUBST(MOTIF_LD_LDARC)
AC_SUBST(PYTHON_INCLUDE_DIR)
AC_SUBST(DEF_PYLIB)
AC_SUBST(NEED_SEMUN)
AC_SUBST(RELEASE_MAJOR)
AC_SUBST(RELEASE_MINOR)
AC_SUBST(RELEASE_RELEASE)
AC_SUBST(RELEASE_COMMIT)
AC_SUBST(RELEASE_BRANCH)
AC_SUBST(RELEASE_TAG)
AC_SUBST(RELEASE_DATE)
AC_SUBST(REMCAM)
AC_SUBST(REMCAMLIB)
AC_SUBST(ENABLE_SANITIZE)
AC_SUBST(SHARETYPE)
AC_SUBST(SHARETYPEJNI)
AC_SUBST(SHARETYPEMOD)
AC_SUBST(SRBLIB)
AC_SUBST(SRBINCLUDE)
AC_SUBST(SYBASE_INC)
AC_SUBST(SYBASE_LIB)
AC_SUBST([TARGET_ARCH])
AC_SUBST(TAR_EXCLUDE)
AC_SUBST(TDIC)
AC_SUBST([THREAD])
AC_SUBST(UIL)
AC_SUBST(XM_LIBS)
AC_SUBST(uiddir)
AC_SUBST(USE_PIPED_MESSAGING)
AC_SUBST(USE_TM_GMTOFF)
AC_SUBST(DC1394_SUPPORT)
AC_SUBST(DC1394_SUPPORT2)
AC_SUBST(VS)
AC_SUBST(VS_ARCH)
AC_SUBST(VS_DIR)
AC_CONFIG_HEADER(include/mdsplus/mdsconfig.h)


AC_OUTPUT(Makefile.inc
          Makefile
          docs/Makefile
          envsyms
          actions/Makefile
          camshr/Makefile
          ccl/Makefile
          d3dshr/Makefile
          dwscope/Makefile
          hdf5/Makefile
          idlmdsevent/Makefile
          idlmdswidgets/Makefile
          javaclient/Makefile
          javadispatcher/Makefile
          javamds/Makefile
          javascope/Makefile
          javatraverser/Makefile
          javatraverser2/Makefile
          javatraverser2/java/mds/mdslib/Makefile
          javadevices/Makefile
          LabView/Makefile
          manpages/Makefile
          macosx/Makefile
          math/Makefile
          mdsdcl/Makefile
	  mdsdcl/mdsdcl_show_version.c
          mdslib/Makefile
          mdslib/docs/Makefile
          mdslib/testing/Makefile
          mdslibidl/Makefile
          mdsobjects/cpp/Makefile
          mdsobjects/cpp/docs/Makefile
          mdsobjects/cpp/testing/Makefile
          mdsobjects/cpp/testing/testutils/Makefile
	  mdsobjects/python/_version.py
          mdsobjects/python/docs/Makefile
          mdsobjects/python/tests/Makefile
          mdsobjects/java/Makefile
          mdsobjects/java/docs/Makefile
          mdsobjects/labview/Makefile
          mdsmisc/Makefile
          mdsshr/Makefile
          mdsshr/testing/Makefile
	  mdsshr/docs/Makefile
          mdssql/Makefile
          mdstcpip/Makefile
          mdstcpip/zlib/Makefile
          mdstcpip/docs/Makefile
          mdstcpip/docs/img/Makefile
          mdsvme/Makefile
          mitdevices/Makefile
	  pydevices/MitDevices/_version.py
	  pydevices/RfxDevices/_version.py
	  pydevices/W7xDevices/_version.py
          remcam/Makefile
          roam/Makefile
          rpm/Makefile
          rpm/envsyms
          rpm/post_install_script
          scripts/Makefile
          servershr/Makefile
          setevent/Makefile
          tcl/Makefile
          tdishr/Makefile
          tdishr/testing/Makefile
          tdic/Makefile
          tditest/Makefile
          tditest/testing/Makefile
          testing/Makefile
          testing/backends/check/Makefile
          testing/selftest/Makefile
          traverser/Makefile
          treeshr/Makefile
          treeshr/testing/Makefile
          wfevent/Makefile
          xmdsshr/Makefile
          xtreeshr/Makefile
          icons/Makefile
)




AC_OUTPUT_COMMANDS(make depend)
if test "$JAVA_APS" = ""
then
  AC_MSG_WARN()
  AC_MSG_WARN()
  AC_MSG_WARN(The development environment for Java applications was not found.)
  AC_MSG_WARN(The build of Java applications will be skipped from the build of MDSplus.)
  AC_MSG_WARN(Install a Java SDK and reconfigure to build the Java apps.)
  AC_MSG_WARN()
fi
if test "$LIBREADLINE" = ""
then
  AC_MSG_WARN()
  AC_MSG_WARN()
  AC_MSG_WARN(The libreadline library was not found)
  AC_MSG_WARN(The tdic application will be skipped.)
  AC_MSG_WARN(Install the package containing the readline library (perhaps libreadline-devel) and reconfigure to build the tdic application.)
  AC_MSG_WARN()
  AC_MSG_WARN()
fi
if test "$SYBASE" = ""
then
  AC_MSG_WARN()
  AC_MSG_WARN()
  AC_MSG_WARN(The open sybase software was not found in /usr/local/sybase.)
  AC_MSG_WARN(The sqlserver interface libraries will be skipped.)
  AC_MSG_WARN()
  AC_MSG_WARN()
fi
if test "$HAVE_MOTIF" = "" -a "$MINGW" = "" -a ! "$OS" = "windows"
then
  AC_MSG_WARN()
  AC_MSG_WARN()
  AC_MSG_WARN(The openmotif development environment was not found.)
  AC_MSG_WARN(The openmotif based applications and libraries will be skipped.)
  AC_MSG_WARN()
  AC_MSG_WARN()
fi
if test "$with_idl" = "no"
then
  AC_MSG_WARN()
  AC_MSG_WARN()
  AC_MSG_WARN(The IDL application directory was not found.)
  AC_MSG_WARN(The libraries used for interfacing IDL with MDSplus will be skipped.)
  AC_MSG_WARN()
  AC_MSG_WARN()
fi
if test "$LV" = ""
then
  AC_MSG_WARN()
  AC_MSG_WARN()
  AC_MSG_WARN("Labview include files were not found.)
  AC_MSG_WARN("The labview MDSobjects will not be built")
  AC_MSG_WARN()
  AC_MSG_WARN()
fi
