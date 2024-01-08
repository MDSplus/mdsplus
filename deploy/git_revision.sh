#!/bin/bash
SRC=$(realpath "$(dirname "$0")/..")
GIT="git --git-dir=${SRC}/.git --work-tree=${SRC}"
GIT_TAG="$(${GIT} describe --tag)"
if [ -z "${GIT_BRANCH}" ]; then
	GIT_BRANCH="$(${GIT} rev-parse --abbrev-ref HEAD)"
fi
GIT_REMOTE="$(${GIT} config branch.${GIT_BRANCH}.remote)"
GIT_REMOTE_URL="$(${GIT} config remote.${GIT_REMOTE}.url)"
GIT_COMMIT="$(${GIT} rev-parse HEAD)"
GIT_COMMIT_DATE="$(${GIT} log -1 --format=%ad)"

if test -z "${GIT_REMOTE}"; then
	GIT_REMOTE=LOCAL
	GIT_REMOTE_URL="file:/${SRC}"
fi

RELEASE_TAG="${GIT_TAG}"
AWK=$(which awk)
if test -n "${AWK}"; then
	if test -n "${RELEASE_VERSION}"; then
		RELEASE_MAJOR="$(echo ${RELEASE_VERSION} | ${AWK} -F"." '{ print $1 }')"
		RELEASE_MINOR="$(echo ${RELEASE_VERSION} | ${AWK} -F"." '{ print $2 }')"
		RELEASE_RELEASE="$(echo ${RELEASE_VERSION} | ${AWK} -F"." '{ print $3 }')"
	fi
	if test -z "${RELEASE_MAJOR}" -o -z "${RELEASE_MINOR}" -o -z "${RELEASE_RELEASE}"; then
		RELEASE_MAJOR="$(echo ${RELEASE_TAG} | ${AWK} -F'-' '{ print $2 }')"
		RELEASE_MINOR="$(echo ${RELEASE_TAG} | ${AWK} -F'-' '{ print $3 }')"
		RELEASE_RELEASE="$(echo ${RELEASE_TAG} | ${AWK} -F'-' '{ print $4 }')"
	fi
fi
if test -z "${RELEASE_MAJOR}" -o -z "${RELEASE_MINOR}" -o -z "${RELEASE_RELEASE}"; then
	RELEASE_MAJOR=0
	RELEASE_MINOR=0
	RELEASE_RELEASE=0
fi
RELEASE_VERSION="${RELEASE_MAJOR}.${RELEASE_MINOR}.${RELEASE_RELEASE}"
RELEASE_DATE="$(date)"
# patch branch: maxlength = 12
RELEASE_BRANCH=${BRANCH::12}
if test -n "${BRANCH::12}"
then RELEASE_BRANCH=${BRANCH::12}
elif test -n "${GIT_BRANCH::12}"
then RELEASE_BRANCH=${GIT_BRANCH::12}
else RELEASE_BRANCH=unknown
fi
RELEASE_TAG="${RELEASE_BRANCH}_release-${RELEASE_MAJOR}-${RELEASE_MINOR}-${RELEASE_RELEASE}"


cat - >"${SRC}/m4/ax_git_revision.m4" <<EOF
# this file has been generated by '$0'

AC_DEFUN([AX_GIT_REVISION],[
	AC_PUSH_LOCAL([ax_git_revision])
    GIT_DEFINE_TARGETS
	AS_VAR_SET([GIT_TAG],[$GIT_TAG])
	AS_VAR_SET([GIT_BRANCH],[$GIT_BRANCH])
	AS_VAR_SET([GIT_REMOTE],[$GIT_REMOTE])
	AS_VAR_SET([GIT_REMOTE_URL],[$GIT_REMOTE_URL])
	AS_VAR_SET([GIT_COMMIT],[$GIT_COMMIT])
	AS_VAR_SET([GIT_COMMIT_DATE],["$GIT_COMMIT_DATE"])
	AS_VAR_SET([RELEASE_TAG],[$RELEASE_TAG])
	AS_VAR_SET([RELEASE_BRANCH],[$RELEASE_BRANCH])
	AS_VAR_SET([RELEASE_VERSION],[$RELEASE_VERSION])
	AS_VAR_SET([RELEASE_MAJOR],[$RELEASE_MAJOR])
	AS_VAR_SET([RELEASE_MINOR],[$RELEASE_MINOR])
	AS_VAR_SET([RELEASE_RELEASE],[$RELEASE_RELEASE])
	AS_VAR_SET([RELEASE_DATE],["$RELEASE_DATE"])
	AC_POP_LOCAL([ax_git_revision])
])


AC_DEFUN_LOCAL([ax_git_revision],[GIT_DEFINE_TARGETS],[
AS_VAR_READ([AX_GIT_REVISION_TARGETS],[

# //////////////////////////////////////////////////////////////////////////// #
# //// GIT REVISION TARGETS  ///////////////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #

# ///////////// #
# GIT VARIABLES #
# ///////////// #
GIT_TAG         = \${GIT_TAG}
GIT_BRANCH      = \${GIT_BRANCH}
GIT_REMOTE      = \${GIT_REMOTE}
GIT_REMOTE_URL  = \${GIT_REMOTE_URL}

release_tag: ##@@miscellaneous dump release tag
	@echo "\\\$(RELEASE_TAG)"

git_info: ##@@miscellaneous dump release tag
	@ \\\$(if \\\$(_git_check_changes),\\\$(info  WARNING: There are uncommitted changes))\\
	  echo " Git repository status: "; \\
	  echo " ------------------------------ "; \\
	  echo "";
	  echo "   tag: ...... \\\$(GIT_TAG)"; \\
	  echo "   branch: ... \\\$(GIT_BRANCH)"; \\
	  echo "   commit: ... \\\$(GIT_COMMIT)"; \\
	  echo "   remote_url: \\\$(GIT_REMOTE_URL)"; \\
	  echo "";

])
AC_SUBST([AX_GIT_REVISION_TARGETS])
m4_ifdef([AM_SUBST_NOTMAKE], [AM_SUBST_NOTMAKE([AX_GIT_REVISION_TARGETS])])
])
EOF
