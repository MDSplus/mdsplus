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
    ./trigger.sh [--test[=skip]] [--valgrind=skip] [--sanitize=skip] [--test_format=tap|log] 
                 [--release] [--releasedir=directory] 
                 [--publish] [--publishdir=directory]
                 [--keys=dir] [--dockerpull] [--color]
                 

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

   --test[=skip]
       Build and test the mdsplus sources. The type of tests attempted
       can be controlled by the --valgrind and --sanitize options. Some
       cross-compiled platforms cannot be tested so the os.opt file will
       contain a --test=skip which supercedes a --test option on the
       command line.

    --valgrind=test-list|skip
       Normally the build jobs will perform valgrind tests when testing
       using the test suite that the platform supports. If this is set
       to skip then no valgrind tests will be performed by the build jobs.

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

EOF
}
opts=""
parsecmd() {
    for i in $1
    do
	case $i in
	    --help|-h|-\?)
		printhelp
		exit
		;;
	    --test)
		opts="${opts} ${i}"
		;;
	    --test=skip)
		opts="${opts} ${i}"
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
	    --valgrind=*)
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
		;;
	    --dockerpull)
		opts="${opts} ${i}"
		;;
	    --color)
		opts="${opts} ${i}"
		COLOR=yes
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

SRCDIR=$(realpath $(dirname ${0})/..)

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

BRANCH=${GIT_BRANCH:7}
opts="$opts --branch=$BRANCH"

if [ "$RELEASE" = "yes" ]
then
    RELEASE_TAG=$(git tag | grep ${BRANCH}_release | sort -V | awk '{line=$0} END{print line}');
    if [ -z ${RELEASE_TAG} ]
    then
	RELEASE_TAG="${BRANCH}_release-1-0-0"
    fi
    MAJOR=$(echo $RELEASE_TAG | cut -f2 -d-);
    MINOR=$(echo $RELEASE_TAG | cut -f3 -d-);
    RELEASEV=$(echo $RELEASE_TAG | cut -f4 -d-);
    if [ "$BRANCH" = "stable" ]
    then
	BNAME=""
    else
	BNAME="-${BRANCH}"
    fi
    if [ "$(git rev-list -n 1 $RELEASE_TAG)" != "${GIT_COMMIT}" ]
    then
	let RELEASEV=$RELEASEV+1;
	RELEASE_TAG=${BRANCH}_release-${MAJOR}-${MINOR}-${RELEASEV};
    fi
    RELEASE_VERSION=${MAJOR}.${MINOR}.${RELEASEV}
    git log --decorate=full > ${SRCDIR}/ChangeLog
    opts="$opts --release=${RELEASE_VERSION} --gitcommit=${GIT_COMMIT}"
    cat <<EOF > ${SRCDIR}/trigger.version
RELEASE_TAG=${RELEASE_TAG}
RELEASE_VERSION=${RELEASE_VERSION}
EOF
fi
if [ "$PUBLISH" = "yes" ]
then
    if [ -r ${SRCDIR}/trigger.version ]
    then
	. ${SRCDIR}/trigger.version
	opts="$opts --publish=${RELEASE_VERSION}"
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
	comment="New release of ${BRANCH} branch of MDSplus." \
	       "Version mdsplus${BNAME}-${MAJOR}-${MINOR}-${RELEASE}"
	git tag -f -a -m  "${comment}" ${RELEASE_TAG};
	git push --follow-tags origin ${BRANCH};
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
fi
echo $opts > ${SRCDIR}/trigger.opts

