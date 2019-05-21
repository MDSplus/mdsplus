# ===========================================================================
#      http://www.gnu.org/software/autoconf-archive/ac_git_revision.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_GIT_REVISION([git_caommand])
#
# DESCRIPTION
#  Adds Git variables to code that can be used to locate your current install
#  upon the source repository that triggered the "make install" command
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

# //////////////////////////////////////////////////////////////////////////// #
# ////// GIT INFO //////////////////////////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #

AC_DEFUN([AX_GIT_REVISION],[
    AC_PUSH_LOCAL([ax_git_revision])

    AC_ARG_ENABLE(uncommitted-install, [AS_HELP_STRING([--disable-uncommitted-install],
                                          [disable install target if there are uncommitted changes])])
    AS_IF([test -z "${enable_uncommitted_install}"], [AS_VAR_SET([enable_uncommitted_install],[yes])])
    AS_VAR_IF([enable_uncommitted_install],[yes],
        [AS_VAR_SET([enable_uncommitted_install],[#])],
        [AS_VAR_SET([enable_uncommitted_install],[])])

    AC_CHECK_PROG([HAVE_GIT],[git],[yes],[no])
    AS_IF([test -d ${srcdir}/.git],,AS_VAR_SET([HAVE_GIT],[no]))    
    
    AS_VAR_SET([abs_top_srcdir],[$(cd ${srcdir}; pwd)])
    AS_IF([test -d ${abs_top_srcdir}],,AS_VAR_SET([HAVE_GIT],[no]))    

    AS_VAR_IF([HAVE_GIT],[yes],[
        AS_VAR_SET([GIT],["git --git-dir=${abs_top_srcdir}/.git --work-tree=${abs_top_srcdir} "])
        AS_VAR_SET([GIT_TAG],[$(${GIT} describe --tag)])
        AS_VAR_SET([GIT_BRANCH],[$(${GIT} rev-parse --abbrev-ref HEAD)])
        AS_VAR_SET([GIT_REMOTE],[$(${GIT} config branch.${GIT_BRANCH}.remote)])
        AS_VAR_SET([GIT_REMOTE_URL],[$(${GIT} config remote.${GIT_REMOTE}.url)])
        AS_VAR_SET([GIT_COMMIT],[$(${GIT} rev-parse HEAD)]) dnl add --short to have 7 digit code
        AS_VAR_SET([GIT_COMMIT_DATE], [$(${GIT} log -1 --format="%ad")])
        AS_VAR_SET([GIT_SRCDIR],[${abs_top_srcdir}])

        ## remoute could not be set to local branch
        AS_IF([test -z "${GIT_REMOTE}"], [
            AS_VAR_SET([GIT_REMOTE],[LOCAL])
            AS_VAR_SET([GIT_REMOTE_URL],["source code upstream is not set, please refer to local srcdir."])
        ])
        
        ## substs
        GIT_DEFINE_TARGETS
        
        dnl TODO: verify matching TAG regexp
        AC_CHECK_PROG([HAVE_AWK],[awk],[yes],[no])
        AS_VAR_IF([HAVE_AWK],[yes],[
            AS_IF([test -z "${RELEASE_VERSION}"],[
                AS_VAR_SET([RELEASE_TAG],[$(echo ${GIT_TAG} | ${AWK} '{ match($[]0,/([[a-zA-Z0-9_-]]+[[^0-9-]]+)-([[0-9]]+-[[0-9]]+-[[0-9]]+)/,arr); print arr[[0]] }' )])
                AS_VAR_SET([RELEASE_VERSION],[$(echo ${GIT_TAG} | ${AWK} '{ match($[]0,/[[0-9]]+-[[0-9]]+-[[0-9]]+/,arr); print arr[[0]] }' | ${AWK} '{ gsub("-","."); print }' ) ])
                AS_VAR_SET([RELEASE_MAJOR],  [$(echo ${RELEASE_VERSION} | ${AWK} -F"." '{ print $[]1 }' ) ])
                AS_VAR_SET([RELEASE_MINOR],  [$(echo ${RELEASE_VERSION} | ${AWK} -F"." '{ print $[]2 }' ) ])
                AS_VAR_SET([RELEASE_RELEASE],[$(echo ${RELEASE_VERSION} | ${AWK} -F"." '{ print $[]3 }' ) ])
                AS_VAR_SET([RELEASE_DATE],   [$(${GIT} log -1 --format="%ad")])
        ])])
    ])
    AC_POP_LOCAL([ax_git_revision])
])




AC_DEFUN_LOCAL([ax_git_revision],[GIT_DEFINE_TARGETS],[
AS_VAR_READ([AX_GIT_REVISION_TARGETS],[

# //////////////////////////////////////////////////////////////////////////// #
# //// GIT REVISION TARGETS  ///////////////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #
 
# check for uncommitted modified files and eventually define "M" for the GIT_TAG
_git_check_changes = \$(shell ${GIT} diff-index --quiet HEAD || echo M)

# check if the current branch name matches the autoconf variable and trigger a reconfigure otherwise
\$(shell test "$(${GIT} rev-parse --abbrev-ref HEAD)" = "${GIT_BRANCH}" || touch \$(top_builddir)/config.status)

# ///////////// #
# GIT VARIABLES #
# ///////////// #
GIT_TAG         = ${GIT_TAG}\$(_git_check_changes)
GIT_SRCDIR      = \$(abs_top_srcdir)
GIT_BRANCH      = ${GIT_BRANCH} 
GIT_REMOTE      = ${GIT_REMOTE}
GIT_REMOTE_URL  = ${GIT_REMOTE_URL}

release_tag: ##@@miscellaneous dump release tag
	@ \$(if \$(_git_check_changes),\$(info "WARNING: There are uncommitted changes")) \
      echo "\$(RELEASE_TAG)"

git_info: ##@@miscellaneous dump release tag
	@ \$(if \$(_git_check_changes),\$(info  WARNING: There are uncommitted changes))\
	  echo " Git repository status: "; \
	  echo " ------------------------------ "; \
	  echo "";
	  echo "   tag: ...... \$(GIT_TAG)"; \
	  echo "   branch: ... \$(GIT_BRANCH)"; \
	  echo "   commit: ... \$(GIT_COMMIT)"; \
	  echo "   remote_url: \$(GIT_REMOTE_URL)"; \
	  echo "";
      

ifeq (install,\$(filter install,\$(MAKECMDGOALS)))
${enable_uncommitted_changes}\$(if \$(_git_check_changes),\$(error ERROR: There are uncommitted changes, install not allowed))
endif

])
AC_SUBST([AX_GIT_REVISION_TARGETS])
m4_ifdef([AM_SUBST_NOTMAKE], [AM_SUBST_NOTMAKE([AX_GIT_REVISION_TARGETS])])
])








dnl ////////////////////////////////////////////////////////////////////////////
dnl ////////////////////////////////////////////////////////////////////////////
dnl ////////////////////////////////////////////////////////////////////////////
dnl // Utility functions

AC_DEFUN_LOCAL([ax_git_revision],[AS_BANNER],[
          AS_ECHO
          AS_BOX([// $1 //////], [\/])
          AS_ECHO
         ])


AC_DEFUN_LOCAL([ax_git_revision],[AS_VAR_READ],[
read -d '' $1 << _as_read_EOF
$2
_as_read_EOF
])
