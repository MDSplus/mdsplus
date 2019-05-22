# ===========================================================================
#      http://www.gnu.org/software/autoconf-archive/ax_reconfigure_targets.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_RECONFIGURE_TARGETS()
#
# USAGE
#   Add @AX_RECONFIGURE_TARGET@ in Makefile.in after standard directories definition
#
# DESCRIPTION
#   Add auto reconfigure targets that trigger a reconfigure action upon changes on
#   config.status file and configure script itself. This emulate the Automake config
#   behavior also in pure autoconf Makefiles.
#   IMPORTANT NOTE:
#     The implementation needs the srcdir, builddir, top_srcdir etc ... to be set
#     before the @AX_RECONFIGURE_TARGET@ call in any Makefile.in
#
# LICENSE
#
#   Copyright (c) 2016 Andrea Rigoni Garola <andrea.rigoni@igi.cnr.it>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

#serial 1

AC_DEFUN([AX_RECONFIGURE_TARGETS],[
    AC_PUSH_LOCAL([ax_reconifgure_targets])
    AC_ARG_ENABLE(reconfigure, [AS_HELP_STRING([--disable-reconfigure],[disable reconfigure targets for autoconf])])
    AS_IF([test -z "${enable_reconfigure}"], [AS_VAR_SET([enable_reconfigure],[yes])])
    AS_VAR_IF([enable_reconfigure],[yes],[
        SET_RECONFIGURE_TARGET
    ])
    AC_POP_LOCAL([ax_reconifgure_targets])
])





AC_DEFUN_LOCAL([ax_reconifgure_targets],[SET_RECONFIGURE_TARGET],[
AS_VAR_READ([AX_RECONFIGURE_TARGET],[

# trigger reconfigure if config.status has been touched
Makefile: \$(srcdir)/Makefile.in \$(top_builddir)/config.status
	cd '\$(top_builddir)' && \$(SHELL) ./config.status;

# trigger reconfigure at configure changes
\$(top_builddir)/config.status: \$(top_srcdir)/configure
	\$(info -------------------------------------------------------------------------------------)\
	\$(info INFO: A reconfigure is needed because the configure does not match with config.status)\
	\$(info -------------------------------------------------------------------------------------)\
	cd '\$(top_builddir)' && \$(SHELL) ./config.status --recheck;

])
AC_SUBST([AX_RECONFIGURE_TARGET])
m4_ifdef([AM_SUBST_NOTMAKE], [AM_SUBST_NOTMAKE([AX_RECONFIGURE_TARGET])])
])

















dnl ////////////////////////////////////////////////////////////////////////////
dnl ////////////////////////////////////////////////////////////////////////////
dnl ////////////////////////////////////////////////////////////////////////////
dnl // Utility functions

AC_DEFUN_LOCAL([ax_reconifgure_targets],[AS_BANNER],[
          AS_ECHO
          AS_BOX([// $1 //////], [\/])
          AS_ECHO
         ])


AC_DEFUN_LOCAL([ax_reconifgure_targets],[AS_VAR_READ],[
read -d '' $1 << _as_read_EOF
$2
_as_read_EOF
])
