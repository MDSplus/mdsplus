#!/bin/bash
#
# mdsplus-deploy/trigger.sh
#
# Configure a build for generating new releases in jenkins.
# This sets build options to be imported into jenkins build jobs
# for the various operating systems supported.
#
printhelp() {
    cat <<EOF
NAME

    trigger.sh - the MDSplus trigger script to configure build options for related
                 build jobs used to test pull requests and generate new MDSplus releases.

SYNOPSIS
    ./trigger.sh [--test[=skip]] [--test_timeunit=factor] [--testrelease] [--valgrind=skip] [--sanitize=skip] [--test_format=tap|log]
                 [--release] [--releasedir=directory]
                 [--publish] [--publishdir=directory]
                 [--keys=dir] [--dockerpull] [--color]
                 [--pypi] [--make_jars ]

DESCRIPTION
    The trigger.sh script is used in conjunction with platform build jobs
    in jenkins. It's main purpose is to configure a trigger.opts file
    which is imported into the build jobs configuration to select the
    type of builds and tests to perform. This is used by creating a
    trigger job in jenkins which runs this script with a desired configuration,
    triggers builds for the various operating systems, waits for completion,
    if necessary changes the configuration and triggers the builds again.

    For example, to build a new MDSplus release, a new release jenkins job
    is created which runs this script to select testing and release operations,
    trigger all the build jobs, wait for completion, if successful, tag a new
    release, configure for publish and trigger the build jobs again so they
    can publish the new releases they just built.

OPTIONS

   --make_jars=fc25
       Build the java jars file in a build_jars subdirectory of the specified os.
       This will make the directory if necessary, cd to that directory and run
       configure --enable-java_only and build the java jars in that directory
       tree. This is used to build the jar files once and then trigger the
       platform builds with a --jars_dir=directory option so platform builds
       can just cp the jar files from the trigger directory location.

   --test[=skip]
       Build and test the mdsplus sources. The type of tests attempted
       can be controlled by the --valgrind and --sanitize options. Some
       cross-compiled platforms cannot be tested so the os.opt file will
       contain a --test=skip which supercedes a --test option on the
       command line.

    --test_timeunit=factor
       Multiply timeout limits by factor to extend or shorten timeout limits

    --testrelease
       Use for pull request tests so kits will be built and checked but
       release tags won't be used to compute release number.

    --valgrind
       Enable valgrind testing for this build

    --valgrind=test-list|skip
       Normally the build jobs will perform valgrind tests when testing
       using the test suite that the platform supports. If this is set
       to skip then no valgrind tests will be performed by the build jobs.

    --sanitize
       Enable sanitize testing for this build

    --sanitize=test-list|skip
       Similar to the --valgrind=skip option this can be used to prevent
       any sanitize tests from being performed.

   --test_format=tap|log
       Format used when publishing test results. If you want to see the
       output of the tests use the log option. The tap option will publish
       the results in a standard tap format which only gives pass or fail
       information.

   --release
       This option will first produce a ChangeLog file from git history.
       It then will check to see if there are release tags for the
       git branch checked out. If so it will determine if the branch
       was changed since the last release and, if so, increment the
       version number. It will then add a --release=version to the
       trigger.opts so the build jobs will build releases with that
       version. If used on a branch without release tags it will
       use a version of 1.2.3. This could be used during jobs like
       pull request tests to exercise the full build procedure on
       arbitrary git commits.

   --releasedir=dir
       Specified the directoty to place the installers after building
       them and where to get the installers from when using the --publish
       option. (Required if either the --release or --publish options
       are specified.)

    --tag_release
       After a successful release build the branch can be tagged with
       a new release tag marking the sources/commit used to produce the
       new release.

    --publish
       This will tag the commit used for the release build with the
       version and adds a --publish=version option to trigger.opts to
       instruct the build jobs to publish the released version.
       Both the --releasedir and --publishdir options must be provided
       when triggering a publish operation.

    --publishdir=dir
       Specifies the public repository top directory. (Required if
       the --publish option is specified.)

    --dockerpull
       Includes a --dockerpull option in trigger.opts which will force
       all builds based on docker images to pull the latest docker
       images from dockerhub.com.

    --keys=dir
       Specifies a directory containing signing keys and certificates
       use when building official MDSplus installers. Note the keys
       in this directory must be owned by the user executing this
       build script.

    --color
       Output failure and success messages in color using ansi color
       escape sequences.

    --pypi
       Push python package to Python Package Index (http://pypi.python.org/)

EOF
}

SRCDIR=$(realpath $(dirname ${0})/..)

opts=""
parsecmd() {
    for i in $1
    do
	case $i in
	    --help|-h|-\?)
		printhelp
		exit
		;;
	    --make_jars=*)
		MAKE_JARS=${i#*=}
		opts="${opts} --jars-dir=${SRCDIR}/jars"
		;;
	    --test)
		opts="${opts} ${i}"
                ;;
            --testrelease)
                opts="${opts} ${i}"
		;;
	    --test=skip)
		opts="${opts} ${i}"
		;;
	    --test_timeunit=*)
		opts="${opts} --test_timeunit=${i#*=}"
		;;
	    --test_format=*)
		opts="${opts} ${i}"
		;;
	    --release)
		RELEASE=yes
		;;
	    --tag_release)
		TAG_RELEASE=yes
		;;
	    --publish)
		PUBLISH=yes
		;;
	    --valgrind)
		opts="${opts} ${i}"
		;;
	    --valgrind=*)
		opts="${opts} ${i}"
		;;
	    --sanitize)
		opts="${opts} ${i}"
		;;
	    --sanitize=*)
		opts="${opts} ${i}"
		;;
	    --releasedir=*)
		opts="${opts} ${i}"
		;;
	    --publishdir=*)
		opts="${opts} ${i}"
		;;
	    --keys=*)
		opts="${opts} ${i}"
		KEYS=${i#*=}
		;;
	    --dockerpull)
		opts="${opts} ${i}"
		;;
	    --color)
		opts="${opts} ${i}"
		COLOR=yes
		;;
	    --pypi)
		PUSH_TO_PYPI=yes
		;;
	    --promote_to=*)
		PROMOTE_TO=${i#*=}
		;;
	    *)
		unknownopts="${unknownopts} $i"
		;;
	esac
    done
    if [ ! -z "${unknownopts}" ]
    then
	printhelp
	2>&1 echo "Unknown option(s) specified: ${unknownopts}"
	exit 1
    fi
}


cmdopts="$@"

#
# Parse the options provided in the command
#
parsecmd "$cmdopts"

RED() {
    if [ "$1" = "yes" ]
    then
	echo -e "\033[31;47m"
    fi
}
GREEN() {
    if [ "$1" = "yes" ]
    then
	echo -e "\033[32;47m"
    fi
}
NORMAL() {
    if [ "$1" = "yes" ]
    then
	echo -e "\033[m"
    fi
}

if [ ! -z "${MAKE_JARS}" ]
then
    if ( ! ${SRCDIR}/deploy/build.sh --make-jars --os=${MAKE_JARS} --workspace=${SRCDIR} > make_jars.log 2>&1 )
    then
	RED $COLOR
	cat <<EOF >&2
===============================================

Error creating java jar files. Trigger failed. 
Look at make_jars.log artifact for more info .

===============================================
EOF
	NORMAL $COLOR
	exit 1
    fi
fi

if [ "$RELEASE" = "yes" -a "$PUBLISH" = "yes" ]
then
    RED $COLOR
    cat <<EOF >&2
===============================================

Do not use both --release and --publish in the
same trigger. Use --release to trigger creation
of new releases first and if successful follow
with a separate --publish to install the new
release in public release area.
FAILURE

===============================================
EOF
    NORMAL $COLOR
    exit 1
fi

BRANCH=${GIT_BRANCH##*/}
opts="$opts --branch=$BRANCH"

if [ "$BUILD_CAUSE" = "GHPRBCAUSE" ]
then
  if [ $(${SRCDIR}/deploy/commit_type_check.sh origin/$ghprbTargetBranch ${SRCDIR}/deploy/inv_commit_title.msg) = "BADCOMMIT" ]
  then
      RED $COLOR
      cat <<EOF >&2
=========================================================

WARNING: Pull request contains an invalid commit title.

=========================================================
EOF
      NORMAL $COLOR
#      exit 1
  fi
fi

if [ ! -z "${PROMOTE_TO}" ]
then
    if ( ${SRCDIR}/deploy/promote.sh ${BRANCH} ${PROMOTE_TO} )
    then
	PROMOTE_RELEASE_TAG=$(git tag | grep ${PROMOTE_TO}_release | sort -V | awk '{line=$0} END{print line}')
	MAJOR=$(echo $PROMOTE_RELEASE_TAG | cut -f2 -d-)
	MINOR=$(echo $PROMOTE_RELEASE_TAG | cut -f3 -d-)
	RELEASEV=$(echo $PROMOTE_RELEASE_TAG | cut -f4 -d-)
    else
      RED $COLOR
      cat <<EOF >&2
=========================================================

ERROR: Problem promoting ${BRANCH} to ${PROMOTE_TO}

=========================================================
EOF
      NORMAL $COLOR
      exit 1
    fi
fi

#
# Make sure submodules have been updated
#
if [ ! -r ${SRCDIR}/3rd-party-apis/.git ]
then
    ${SRCDIR}/conf/update_submodules
fi

if [ "$RELEASE" = "yes" ]
then
    NEW_RELEASE=no
    if [ -z ${RELEASE_TAG} ]
    then
	RELEASE_TAG=$(git tag | grep ${BRANCH}_release | sort -V | awk '{line=$0} END{print line}');
    fi
    if [ -z ${RELEASE_TAG} ]
    then
	RELEASE_TAG="${BRANCH}_release-1-0-0"
    fi
    if [ -z "${PROMOTE_TO}" ]
    then
	MAJOR=$(echo $RELEASE_TAG | cut -f2 -d-);
	MINOR=$(echo $RELEASE_TAG | cut -f3 -d-);
	RELEASEV=$(echo $RELEASE_TAG | cut -f4 -d-);
    fi
    LAST_RELEASE_COMMIT=$(git rev-list -n 1 $RELEASE_TAG)
    if [ "${LAST_RELEASE_COMMIT}" != "${GIT_COMMIT}" ]
    then
	if [ -z "${PROMOTE_TO}" ]
	then
	    version_inc=$(${SRCDIR}/deploy/commit_type_check.sh "${LAST_RELEASE_COMMIT}" ${SRCDIR}/deploy/inv_commit_title.msg)
	else
	    version_inc=PROMOTE
	fi
	case "$version_inc" in
	    PROMOTE)
		NEW_RELEASE=yes
		;;
	    BADCOMMIT)
		RED $COLOR
		cat <<EOF >&2
=========================================================

WARNING: Commit contains an invalid commit title.

=========================================================
EOF
		NORMAL $COLOR
		#exit 1
		NEW_RELEASE=yes
		let RELEASEV=$RELEASEV+1
		;;
	    SAME)
		GREEN $COLOR
		cat <<EOF >&2
=========================================================

INFO: All commits are of category Build, Docs or Tests.
      No new release generated.

=========================================================

EOF
		NORMAL $COLOR
		NEW_RELEASE=no
		;;
	    MINOR)
		GREEN $COLOR
		cat <<EOF >&2
=========================================================

INFO: New features added. New release will be a minor
      version increase.

=========================================================

EOF
		NORMAL $COLOR
		NEW_RELEASE=yes
		let MINOR=$MINOR+1
		let RELEASEV=0
		;;
	    PATCH)
		GREEN $COLOR
		cat <<EOF >&2
=========================================================

INFO: No new features added. Fix commits added so
      new release will be generated with patch version
      incremented.

=========================================================

EOF
		NORMAL $COLOR
		NEW_RELEASE=yes
		let RELEASEV=$RELEASEV+1
		;;
	    *)
		GREEN $RED
		cat <<EOF >&2
=========================================================

INFO: Unknown release check return of $version_inc
      A patch releases will be created.

=========================================================

EOF
		NORMAL $COLOR
	    	NEW_RELEASE=yes
		let RELEASEV=$RELEASEV+1
		;;
	esac
    fi
    RELEASE_TAG=${BRANCH}_release-${MAJOR}-${MINOR}-${RELEASEV};
    RELEASE_VERSION=${MAJOR}.${MINOR}.${RELEASEV}
    git log --decorate=full --no-merges > ${SRCDIR}/ChangeLog
    opts="$opts --release=${RELEASE_VERSION} --gitcommit=${GIT_COMMIT}"
    cat <<EOF > ${SRCDIR}/trigger.version
NEW_RELEASE=${NEW_RELEASE}
LAST_RELEASE_COMMIT=${LAST_RELEASE_COMMIT}
RELEASE_TAG=${RELEASE_TAG}
RELEASE_VERSION=${RELEASE_VERSION}
EOF
    GREEN $COLOR
    cat <<EOF
=========================================================

Triggering release ${RELEASE_TAG} build

=========================================================
EOF
    NORMAL $COLOR
fi
if [ "$PUBLISH" = "yes" ]
then
    if [ -r ${SRCDIR}/trigger.version ]
    then
	. ${SRCDIR}/trigger.version
	opts="$opts --publish=${RELEASE_VERSION}"
	GREEN $COLOR
	cat <<EOF
=========================================================

Triggering publish release of ${RELEASE_TAG}

=========================================================
EOF
	NORMAL $COLOR
    else
	RED $COLOR
	cat <<EOF >&2
=========================================================

Attempt to publish a new release without first triggering
a release build with the --release option.
FAILURE

=========================================================
EOF
	NORMAL $COLOR
	exit 1
    fi
fi
if [ "$TAG_RELEASE" = "yes" ]
then
    if [ -r ${SRCDIR}/trigger.version ]
    then
	. ${SRCDIR}/trigger.version
	if [ "$NEW_RELEASE" = "yes" ]
	then
	   curl --data @- "https://api.github.com/repos/MDSplus/mdsplus/releases?access_token=$(cat $KEYS/.git_token)" > ${WORKSPACE}/tag_release.log 2>&1 <<EOF
{
  "tag_name":"${RELEASE_TAG}",
  "target_commitish":"${BRANCH}",
  "name":"${RELEASE_TAG}",
  "body":"Commits since last release:\n\n
$(git log --decorate=full --no-merges ${LAST_RELEASE_COMMIT}..HEAD | awk '{gsub("\"","\\\"");print $0"\\n"}')"
}
EOF
	   if ( ! grep tag_name ${WORKSPACE}/tag_release.log > /dev/null )
	   then
	       RED $COLOR
	       cat <<EOF >&2
=========================================================

Failed to tag a new release on github. Response/error
content follows.
FAILURE

=========================================================
EOF
	       cat ${WORKSPACE}/tag_release.log
	       NORMAL $COLOR
	       exit 1
	   fi
	fi
    else
	RED $COLOR
	cat <<EOF >&2
=========================================================

Attempt to tag a new release without first triggering
a release build with the --release option.
FAILURE

=========================================================
EOF
	NORMAL $COLOR
	exit 1
    fi
    if [ "$PUSH_TO_PYPI" = "yes" ]
    then
	export BRANCH RELEASE_VERSION
	if [ "$BRANCH" = "stable" ]
	then
	    pypi_name=MDSplus
	else
	    pypi_name=mdsplus-${BRANCH}
	fi
	if (! wget -O - "https://pypi.python.org/${pypi_name}/$RELEASE_VERSION" >/dev/null 2>&1 )
	then
	    tmpdir=$(mktemp -d)
	    cd $tmpdir
	    ${WORKSPACE}/configure --disable-java >/dev/null 2>&1
	    rsync -a ${WORKSPACE}/mdsobjects/python mdsobjects/
	    cd mdsobjects/python
	    HOME=${KEYS} python setup.py sdist upload
	    cd $WORKSPACE
	    rm -Rf $tmpdir
	fi
    fi
fi
echo $opts > ${SRCDIR}/trigger.opts

