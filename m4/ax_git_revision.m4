# ===========================================================================
#      http://www.gnu.org/software/autoconf-archive/ac_git_revision.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_GIT_REVISION([git_caommand])
#
# DESCRIPTION
#
#
# EXAMPLE
#
#
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

# # //////////////////////////////////////////////////////////////////////////// #
# # ////// RELEASE INFO //////////////////////////////////////////////////////// #
# # //////////////////////////////////////////////////////////////////////////// #

# if [[ ! -z ${RELEASE_VERSION} ]]
# then
#   RELEASE_MAJOR=$(echo ${RELEASE_VERSION} | cut -d. -f1)
#   RELEASE_MINOR=$(echo ${RELEASE_VERSION} | cut -d. -f2)
#   RELEASE_RELEASE=$(echo ${RELEASE_VERSION} | cut -d. -f3)
#   RELEASE_BRANCH=$BRANCH
#   RELEASE_COMMIT=$GIT_COMMIT
# else
#   RELEASE_MAJOR=1
#   RELEASE_MINOR=0
#   RELEASE_RELEASE=0
#   RELEASE_BRANCH="unknown"
#   RELEASE_COMMIT="unknown"
# fi
# RELEASE_DATE="$(date)"
# RELEASE_TAG="${BRANCH}_release-${RELEASE_MAJOR}-${RELEASE_MINOR}-${RELEASE_RELEASE}"


AC_DEFUN([AX_GIT_REVISION],[
    AC_PUSH_LOCAL([ax_git_revision])
    AC_CHECK_PROG([HAVE_GIT],[git],[yes],[no])
    AS_IF([test -d ${srcdir}/.git],,AS_VAR_SET([HAVE_GIT],[no]))    
    
    AC_CHECK_PROG([HAVE_AWK],[awk],[yes],[no])
    AS_VAR_IF([HAVE_AWK],,AS_VAR_SET([HAVE_GIT],[no]))

    AS_VAR_SET([abs_top_srcdir],[$(cd ${srcdir}; pwd)])
    AS_IF([test -d ${abs_top_srcdir}],,AS_VAR_SET([HAVE_GIT],[no]))    

    AS_VAR_IF([HAVE_GIT],[yes],[
        AS_VAR_SET([GIT],["git --git-dir=${abs_top_srcdir}/.git"])
        AS_VAR_SET([GIT_TAG],[$(${GIT} describe --tag)])
        AS_VAR_SET([GIT_BRANCH],[$(${GIT} rev-parse --abbrev-ref HEAD)])
        AS_VAR_SET([GIT_REMOTE],[$(${GIT} config branch.${GIT_BRANCH}.remote)])        
        AS_VAR_SET([GIT_REMOTE_URL],[$(${GIT} config remote.${GIT_REMOTE}.url)])        
        AS_VAR_SET([GIT_COMMIT],[$(${GIT} rev-parse HEAD)]) dnl add --short to have 7 digit code
        ## substs
        GIT_SET_TARGETS

        AC_SUBST([GIT_TAG])
        AC_SUBST([GIT_BRANCH])
        AC_SUBST([GIT_REMOTE])
        AC_SUBST([GIT_REMOTE_URL])
        AC_SUBST([GIT_COMMIT])

        dnl TODO: verify matching TAG regexp
        AS_IF([test -z ${RELEASE_VERSION}],[
            AS_VAR_SET([RELEASE_TAG],[${GIT_TAG}])
            AS_VAR_SET([RELEASE_VERSION],[$(echo ${GIT_TAG} | ${AWK} '{ match($[]0,/[[0-9]]+-[[0-9]]+-[[0-9]]+/,arr); print arr[[0]] }' | ${AWK} '{ gsub("-","."); print }' ) ])
            AS_VAR_SET([RELEASE_MAJOR],  [$(echo ${RELEASE_VERSION} | ${AWK} -F"." '{ print $[]1 }' ) ])
            AS_VAR_SET([RELEASE_MINOR],  [$(echo ${RELEASE_VERSION} | ${AWK} -F"." '{ print $[]2 }' ) ])
            AS_VAR_SET([RELEASE_RELEASE],[$(echo ${RELEASE_VERSION} | ${AWK} -F"." '{ print $[]3 }' ) ])
        ])
        
        AS_VAR_SET([RELEASE_DATE], [$(${GIT} log -1 --format="%ad")])
        
    ])
    
    AC_POP_LOCAL([ax_git_revision])
])



AC_DEFUN_LOCAL([ax_git_revision],[GIT_SET_TARGETS],[
AS_VAR_READ([AX_GIT_REVISION_TARGETS],[

# //////////////////////////////////////////////////////////////////////////// #
# //// GIT REVISION TARGETS  ///////////////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #
#
#
_git_check_changes = \$(shell ${GIT} diff-index --quiet HEAD || echo M)
#
GIT_RELEASE_TAG = ${RELEASE_TAG}\$(_git_check_changes)
GIT_BRANCH      = ${GIT_BRANCH} 
GIT_REMOTE      = ${GIT_REMOTE}
GIT_REMOTE_URL  = ${GIT_REMOTE_URL}
GIT_COMMIT      = ${GIT_COMMIT}
GIT_SRCDIR      = \$(abs_top_srcdir)

git_install_check: ##@miscellaneous check if we are about to install uncommitted changes
	@ \$(if \$(_git_check_changes),\$(info "WARNING: There are uncommitted changes")):;

git_release_tag: ##@miscellaneous dump release tag
	@ echo "\$(GIT_RELEASE_TAG)"

git_show_log:
	@ ${GIT} log

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
