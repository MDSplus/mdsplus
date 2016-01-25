

dnl FROM /usr/share/autoconf/autoconf/general.m4
dnl 
dnl AC_SUBST([bindir],         ['${exec_prefix}/bin'])dnl
dnl AC_SUBST([sbindir],        ['${exec_prefix}/sbin'])dnl
dnl AC_SUBST([libexecdir],     ['${exec_prefix}/libexec'])dnl
dnl AC_SUBST([datarootdir],    ['${prefix}/share'])dnl
dnl AC_SUBST([datadir],        ['${datarootdir}'])dnl
dnl AC_SUBST([sysconfdir],     ['${prefix}/etc'])dnl
dnl AC_SUBST([sharedstatedir], ['${prefix}/com'])dnl
dnl AC_SUBST([localstatedir],  ['${prefix}/var'])dnl
dnl AC_SUBST([includedir],     ['${prefix}/include'])dnl
dnl AC_SUBST([oldincludedir],  ['/usr/include'])dnl
dnl AC_SUBST([docdir],         [m4_ifset([AC_PACKAGE_TARNAME],
dnl				     ['${datarootdir}/doc/${PACKAGE_TARNAME}'],
dnl				     ['${datarootdir}/doc/${PACKAGE}'])])dnl
dnl AC_SUBST([infodir],        ['${datarootdir}/info'])dnl
dnl AC_SUBST([htmldir],        ['${docdir}'])dnl
dnl AC_SUBST([dvidir],         ['${docdir}'])dnl
dnl AC_SUBST([pdfdir],         ['${docdir}'])dnl
dnl AC_SUBST([psdir],          ['${docdir}'])dnl
dnl AC_SUBST([libdir],         ['${exec_prefix}/lib'])dnl
dnl AC_SUBST([localedir],      ['${datarootdir}/locale'])dnl
dnl AC_SUBST([mandir],         ['${datarootdir}/man'])dnl



AC_DEFUN([AX_MDSPLUS_DIRECTORIES],[


AS_VAR_IF([exec_prefix],["NONE"], 
  AS_VAR_SET([exec_prefix],["${prefix}"]))

AS_VAR_IF([bindir],["\${exec_prefix}/bin"], 
  AS_VAR_SET([bindir],["${exec_prefix}/bin"]))

AS_VAR_IF([sbindir],["\${exec_prefix}/sbin"], 
  AS_VAR_SET([sbindir],["${exec_prefix}/sbin"]))

AS_VAR_IF([libexecdir],["\${exec_prefix}/libexec"], 
  AS_VAR_SET([libexecdir],["${exec_prefix}/libexec"]))

AS_VAR_IF([datarootdir],["\${prefix}/share"], 
  AS_VAR_SET([datarootdir],["${prefix}/share"]))

AS_VAR_IF([datadir],["\${datarootdir}"], 
  AS_VAR_SET([datadir],["${datarootdir}"]))

AS_VAR_IF([sysconfdir],["\${prefix}/etc"], 
  AS_VAR_SET([sysconfdir],["${prefix}/etc"]))

AS_VAR_IF([sharedstatedir],["\${prefix}/com"], 
  AS_VAR_SET([sharedstatedir],["${exec_prefix}/com"]))

AS_VAR_IF([localstatedir],["\${prefix}/var"], 
  AS_VAR_SET([localstatedir],["${prefix}/var"]))

AS_VAR_IF([includedir],["\${prefix}/include"], 
  AS_VAR_SET([includedir],["${prefix}/include"]))



AS_VAR_IF([libdir],["\${exec_prefix}/lib"], 
  AS_VAR_SET([libdir],["${exec_prefix}/lib"]))

AS_VAR_IF([localedir],["\${datarootdir}/locale"], 
  AS_VAR_SET([localedir],["${datarootdir}/locale"]))

AS_VAR_IF([mandir],["\${datarootdir}/man"], 
  AS_VAR_SET([mandir],["${datarootdir}/man"]))


])



