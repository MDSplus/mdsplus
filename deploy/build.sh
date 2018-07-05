#!/bin/bash
#
# mdsplus-deploy/build.sh
#
# Build mdsplus for a selected operating system for testing or
# generating installers.
#
printhelp() {
    cat <<EOF
NAME

    build.sh - the MDSplus build script to build, test and deploy
               installers.

SYNOPSIS
    ./build.sh --os=name [--test[=skip]] [--test_format=list]
               [--testrelease]
               [--release=version] [--publish=version]
               [--source=dir] [--workspace=dir] [--branch=name]
               [--releasedir=dir] [--publishdir=dir] [--keys=dir]
               [--distname=dir] [--dockerpull]
               [--valgrind=test-list] [--sanitize=test-list]
               [--distname=name] [--updatepkg] [--eventport=number]
               [--arch=name] [--color] [--winhost=hostname]
               [--winbld=dir] [--winrembld=dir] [--gitcommit=commit]
               [--jars-dir=dir] [--make-jars] [--docker-srcdir=dir]

DESCRIPTION
    The build.sh script is used for building, testing and deploy MDSplus
    releases. While it is mostly used by the jenkins automated build
     system for MDSplus it can also be used manually to test the builds
     of MDSplus on the supported platforms or to test the MDSplus
     deployment scripts in the mdsplus-deploy git package.

    The build.sh script will only build MDSplus for one target operating
    system per invocation of the script. One must specify whether you
    want to perform the standard tests on the MDSplus software, build
    the installers, or publish the installers to a public repository
    using the --test, --release=version and/or --publish=version command
    options. It is possible to do one or more of these operations in a
    single invocation but to perform the --publish option you must have
    first completed a --release operation. When used in the jenkins
    environment normally a trigger job is used to trigger parallel build
    jobs. Add trigger jobs will add a trigger.opt file which will
    contain default options which are prefixed to the command options
    provided by the command line. Similarly when you specify an
    --os=name option, some default command options for that operating
    system will be prepended to the command line options. This is useful
    for adding valgrind and sanitize test options which are supported by
    the operating system and to specify the standard docker image that
    can be used for that operating system.

    To test changes made to the mdsplus sources completely including
    the packaging, you can do the following:

        $ cd work-directory
        $ mdsplus-src-dir/deploy/build.sh --os=fc22 --test --release [--publish]

    This will build mdsplus, run the entire test suite, build installers and
    verify the contents of the installers.

    If you have changed the packaging, for example adding a new library, header,
    tdi function or any other file that would be included in an installer the
    above test will fail. You will need to issue the following commands to
    update the package contents checking files to refleact the changes.
    issue the following commands:

    $ cd work-directory
    $ mdsplus-src-dir/deploy/build.sh --os=fc22 --release --updatepkg

    After doing the above commands you should use git diff on the package content
    file(s) that were changed and verify that the new contents is correct. These
    updated content files will need to be committed to enable release builds to
    succeed.

    It is recommended that the above build.sh commands be performed on a variety
    of operating system types to ensure successful builds and packaging before
    pushing commits to github.

OPTIONS
   --os=name
       Specify the target operating system you are attempting to build
       for. Supported os names can be found by looking in
       mdsplus/deploy/os. The os name is the opt file name without the
       .opt portion of the options file.

   --test[=skip]
       Build and test the mdsplus sources. The type of tests attempted
       can be controlled by the --valgrind and --sanitize options. Some
       cross-compiled platforms cannot be tested so the os.opt file will
       contain a --test=skip which supercedes a --test option on the
       command line.

   --testrelease
       After performing tests continue to build a test release to verify
       package contents matches expected contents. Add to one os build for
       each of redhat platform and debian platform. Full release builds
       are not needed for all instances of redhat and debian os when just
       performing tests.

   --test_format=list
       Format used when publishing test results. If you want to see the
       output of the tests use the log option. The tap option will publish
       the results in a standard tap format which only gives pass or fail
       information.

    --test_timeunit=float-point-number
       Multiplier to apply to tests timeouts. If this option is used the
       timeout time for a test will be set to this value times the normal
       timeout for the test. For example, --test_timeunit=2.5 would
       allow the tests to run 2.5 as long as the normally do before being
       stopped with a timeout error. 

   --eventport=number
       Select a port number to use for udp event tests. If running
       multiple docker containers concurrently doing tests you should
       use different port numbers to avoid event traffic issues.

   --release[=version]
       Build the installers for the target operating system and place
       them in the directory specified by  the --releasedir option.
       If the version is omitted the version will be set to 1.2.3.
       During new release builds the version will be determined from
       release tags. If just doing test builds the version can be omitted.

   --releasedir=dir
       Specified the directoty to place the installers after building
       them and where to get the installers from if using the --publish
       option. Defaults to ${WORKSPACE}/release.

    --publish[=version]
       Publish the installers for a release by moving them from the
       directory specified by the --releasedir option to the directory
       specified by the --publishdir option. If version is omitted
       a version of 1.2.3 will be used. See --release option.

    --publishdir=dir
       Specifies the public repository top directory. The installers
       will be placed in a subdirectory specified in with the --distname
       option. Defaults to ${WORKSPACE}/publish

    --branch=name
       Specifies the release type when building installers. This will
       typically be either stable or alpha. If omitted, usually just
       for test builds, the name will default to your username.

    --workspace=dir
       Specifies the directory where the builds will take place. If not
       specified it will attempt to use the WORKSPACE environment
       variable to specify the location. If WORKSPACE is not defined it
       will use the ./build/os directory.

    --dockerfile=filespec
       Specifies a dockerfile to use for building a docker image.
       Normally the builds will use standard dockerimage for the build
       obtained from dockerhub but you can override this by providing
       your own dockerfile.

    --dockerpull
       If specified a docker image will be updated from dockerhub
       before performing the build. If a docker image is not already
       present on the system it will automatically pulled from dockerhub
       even without the --dockerpull option.

    --keys=dir
       Specifies a directory containing signing keys and certificates
       use when building official MDSplus installers. Note the keys
       in this directory must be owned by the user executing this
       build script.

    --updatepkg
       If this option is specified, the build will update the expected
       package contents lists to reflect the contents of the new installers
       built. This option should be used when adding new features to MDSplus
       that would change the files included in the installers. The
       release builds verify the contents of new installers against these
       content files.

    --color
       If this option is specified, success and failure messages will be
       output using ansi color escape sequences.

    --winhost=hostname
       Specify the windows system host which has a wsgi application used to
       build the Visual Studio version of the cpp objects dll.

    --winbld=directory
       Specify the directory on the linux host with the windows share mounted.

    --winrembld=directory
       Specify the directory on the windows system host of the windows share.

    --gitcommit=commit
       Set by trigger jenkins job representing the commit hash of the sources.

    --jars-dir=dir
       Set by trigger job to indicate that build job should get the java jar
       files from the trigger source directory instead of building them.

    --make-jars
       Triggers the generation of the java programs (.jar files)

    --disable-java
       Do not compile java programs (passes --disable-java to configure)

    --docker-srcdir=dir
       Specify the directory to use inside the docker containers used to build
       the code. If not specified the docker containers will use the
       full directory spec of the parent directory of the build.sh
       on the host.

OPTIONS WITH OS SPECIFIC DEFAULT

   --platform=name
       Specify the platform type that the operating system is a member.
       This option directs the build system to use build scripts
       specific to a type of platform. For example, a platform of
       'redhat' will use scripts to build, test and create rpm
       installers. If a supported operating system is specified with the
       --os option a default --platform option will be used unless
       overridden by adding this option to the command line


    --distname=name
       Specifies the subdirectory name of the public repository directory
       hierarchy where the installers will be to during the --publish
       operation. The default --distname option will be set if using a
       supported operating system.

    --valgrind
       Enable valgrind testing for this build.

    --valgrind=tests
       Select a set of valgrind tests to perform on the MDSplus code if
       the --test option is included. This is a comma delimited list of
       tests such as memcheck,helgrind. A value of skip can be used to
       prevent any valgrind tests from being performed. If testing a
       supported operating system a default list of tests supported for
       that operating system will be used unless overriden in the
       command line.

    --sanitize
       Enable sanitize testing for this build.

    --sanitize=tests
       Select a set of sanitize tests to perform on the MDSplus code if
       the --test option is included. This option behaves the same as
       the --valgrind option except the types of tests include address,
       undefined and thread.

    --dockerimage=image
       Specifies a docker image name to use for building the software.
       This could be a comma delimited list for platforms such as ubuntu
       where two docker images are used for building 32-bit and 64-bit
       versions of the software. When building a supported operating
       system a default --dockerimage option will be set.

    --arch=name
       For debian packaging the architecture of the debian packages must be
       specified. If building with multiple architectures this should be
       a list in the same order as the docker images used (also a list).
EOF
}
parsecmd() {
    unset INTERACTIVE
    for i in $1
    do
	case $i in
	    --help|-h|-\?)
		printhelp
		exit
		;;
	    --os=*)
		OS="${i#*=}"
		;;
	    --test)
		if [ "${TEST}" != "skip" ]
		then
		    TEST=yes
		fi
		;;
	    --test=skip)
		TEST=skip
		;;
	    --testrelease)
		TEST_RELEASE=yes
		;;
	    --test_format=*)
		TEST_FORMAT="${i#*=}"
		;;
	    --test_timeunit=*)
		TEST_TIMEUNIT="${i#*=}"
		;;
	    --eventport=*)
		EVENT_PORT="${i#*=}"
		;;
	    --release)
		RELEASE_VERSION=1.2.3
		RELEASE=yes
		;;
            --release=*)
		RELEASE_VERSION="${i#*=}"
		RELEASE=yes
		;;
	    --publish)
		RELEASE_VERSION=1.2.3
		PUBLISH=yes
		;;
            --publish=*)
		RELEASE_VERSION="${i#*=}"
		PUBLISH=yes
		;;
	    --platform=*)
		PLATFORM="${i#*=}"
		;;
	    --valgrind)
		ENABLE_VALGRIND=yes
		;;
	    --valgrind=*)
		if [ "${ENABLE_VALGRIND}" = "yes" ]
		then
		    VALGRIND_TOOLS="${i#*=}"
		fi
		;;
	    --sanitize)
		ENABLE_SANITIZE=yes
		;;
	    --sanitize=*)
		if [ "$ENABLE_SANITIZE" = "yes" ]
		then
		    SANITIZE="${i#*=}"
		fi
		;;
	    --branch=*)
		BRANCH="${i#*=}"
		;;
	    --workspace=*)
		NEW_WORKSPACE="${i#*=}"
		;;
	    --releasedir=*)
		RELEASEDIR="${i#*=}"
		;;
	    --publishdir=*)
		PUBLISHDIR="${i#*=}"
		;;
	    --dockerimage=*)
		DOCKERIMAGE="${i#*=}"
		;;
	    --dockerfile=*)
		DOCKERFILE="${i#*=}"
		;;
	    --keys=*)
		KEYS="${i#*=}"
		;;
	    --distname=*)
		DISTNAME="${i#*=}"
		;;
	    --arch=*)
		ARCH="${i#*=}"
		;;
	    --dockerpull)
		DOCKERPULL=yes
		;;
	    --updatepkg)
		UPDATEPKG=yes
		;;
	    --color)
		COLOR=yes
		;;
	    -i)
		INTERACTIVE="1"
		;;
	    --winhost=*)
		WINHOST="${i#*=}"
		;;
	    --winbld=*)
		WINBLD="${i#*=}"
		;;
	    --winrembld=*)
		WINREMBLD="${i#*=}"
		;;
	    --gitcommit=*)
		GIT_COMMIT="${i#*=}"
		;;
	    --jars-dir=*)
		JARS_DIR="$(realpath ${i#*=})"
		;;
	    --make-jars)
		MAKE_JARS="yes"
		;;
	    --disable-java)
		CONFIGURE_PARAMS="$CONFIGURE_PARAMS --disable-java"
		;;
	    --docker-srcdir=*)
		DOCKER_SRCDIR="${i#*=}"
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
opts="$@"
#
# Parse the options provided in the command
#
parsecmd "$opts"

SRCDIR=$(realpath $(dirname ${0})/..)
if [ -z "${DOCKER_SRCDIR}" ]
then
   DOCKER_SRCDIR="${SRCDIR}"
fi

#
# Get the default options for the OS specified.
#
if [ -r "${SRCDIR}/deploy/os/${OS}.opts" ]
then
    os_opts=$(cat ${SRCDIR}/deploy/os/${OS}.opts)
fi
#
# See if this build was triggered by a trigger job and use
# the command options in the trigger.opts file.
#
if [ -r ${SRCDIR}/trigger.opts ]
then
    trigger_opts="$(cat ${SRCDIR}/trigger.opts)"
fi
#
# Reparse the command using the trigger options, the os options and the command line options.
#
cat <<EOF

Build script executing with the following combined options:

   ${trigger_opts} ${os_opts} ${opts}

EOF
parsecmd "${trigger_opts} ${os_opts} ${opts}"

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

if [ "$RELEASE" = "yes" -o "$PUBLISH" = "yes" ]
then
    if [ -r $PUBLISHDIR/${DISTNAME}/${BRANCH}_${RELEASE_VERSION}_${OS} ]
    then
	GREEN $COLOR
	cat <<EOF
==================================================================

A ${RELEASE_VERSION} ${BRANCH} release already exists for ${OS}.
The build will be skipped.

==================================================================
EOF
	NORMAL $COLOR
	exit 0
    fi
fi

#
# Make sure submodules have been updated
#
if [ ! -r ${SRCDIR}/3rd-party-apis/.git ]
then
    ${SRCDIR}/conf/update_submodules
fi

#
# Make sure one of --test --release=version
# --publish=version options were provided.
#
echo "${ENABLE_SANITIZE}"
echo "${SANITIZE}"
if [ "${TEST}"            != "yes"  \
 -a  "${MAKE_JARS}"       != "yes"  \
 -a  "${RELEASE}"         != "yes"  \
 -a  "${PUBLISH}"         != "yes"  ]
then
    >&2 echo "None of --test --make-jars --release=version --publish=version options specified on the command. Nothing to do!"
    exit 0
fi
#
# Make sure the platform specified is supported.
#
if [ -z "$PLATFORM" ]
then
   >&2 echo "No platform specified. Either the --os=osname or the --platform=pname options must be specified."
   exit 1
fi
if [ "PUBLISH" = "yes" -a -z "$DISTNAME" ]
then
    >&2 echo "Attempting to publish without a --distname=name option."
    exit 1
fi
if [ ! -d "${SRCDIR}/deploy/platform/${PLATFORM}" ]
then
    >&2 echo "Plaform ${PLATFORM} is not supported."
    exit 1
fi
if [ -z "$NEW_WORKSPACE" ]
then
    if [ -z "$WORKSPACE" ]
    then
       WORKSPACE=$(pwd)/build/${OS}/${BRANCH}
    fi
else
    WORKSPACE=$(realpath ${NEW_WORKSPACE})
fi

if [ "$TEST" = "yes" -a "$TEST_RELEASE" = "yes" ]
then
    RELEASE=yes
fi

if [ "$RELEASE" = "yes" -o "$PUBLISH" = "yes" ]
then
    if [ -z "$RELEASEDIR" ]
    then
	RELEASEDIR=${WORKSPACE}/release
    fi
else
    RELEASEDIR=""
fi

if [ "$PUBLISH" = "yes" ]
then
    if [ -z "$PUBLISHDIR" ]
    then
	PUBLISHDIR=${WORKSPACE}/publish
    fi
else
    PUBLISHDIR=""
fi
#
# Convert DOCKERIMAGE and DOCKERFILE options
# from coma delimited to space delimited
#
spacedelim() {
    if [ ! -z "$1" ]
    then
	if [ "$1" = "skip" ]
	then
	    ans=""
	else
	    IFS=',' read -ra ARR <<< "$1"
	    ans="${ARR[*]}"
	fi
    fi
    echo $ans
}

DOCKERFILE="$(spacedelim $DOCKERFILE)"
DOCKERIMAGE="$(spacedelim $DOCKERIMAGE)"
#
# pull docker images if specified.
#
if [ "$DOCKERPULL" = "yes" ]
then
    if [ ! -z "$DOCKERIMAGE" ]
    then
	for img in ${DOCKERIMAGE}
	do
	    docker pull $img
	done
    fi
elif [ ! -z "$DOCKERFILE" ]
then
    #
    # build docker images if specified.
    #
    images=( $DOCKERIMAGE )
    files=( $DOCKERFILE )
    if [ "${#images[@]}" != "${#files[@]}" ]
    then
	>&2 echo "number of dockerfiles does not equal number of dockerimage names"
	exit 1
    fi
    for index in ${!images[@]}
    do
	file=files[$index]
	image=image[$index]
	pushd $(dirname $file)
	docker build -t $image .
	popd
    done
fi
mkdir -p ${WORKSPACE}
if [ -z "$BRANCH" ]
then
    BRANCH=$(whoami)
fi
if [ -z "$EVENT_PORT" ]
then
    EVENT_PORT=4000
fi
if [ -z "$TEST_FORMAT" ]
then
    TEST_FORMAT=log
fi

if [ ! -z "$RELEASEDIR" ]
then
    RELEASEDIR=${RELEASEDIR}/${DISTNAME}
fi

if [ ! -z "$PUBLISHDIR" ]
then
    PUBLISHDIR=${PUBLISHDIR}/${DISTNAME}
fi

#
# Invoke the platform specific build script
#
OS=${OS} \
  TEST=${TEST} \
  TEST_FORMAT=${TEST_FORMAT} \
  TEST_TIMEUNIT=${TEST_TIMEUNIT} \
  EVENT_PORT=${EVENT_PORT} \
  MAKE_JARS=${MAKE_JARS} \
  RELEASE=${RELEASE} \
  PUBLISH=${PUBLISH} \
  RELEASE_VERSION=${RELEASE_VERSION} \
  PLATFORM=${PLATFORM} \
  VALGRIND_TOOLS=${VALGRIND_TOOLS} \
  SANITIZE=${SANITIZE} \
  SRCDIR=${SRCDIR} \
  DOCKER_SRCDIR="$DOCKER_SRCDIR" \
  BRANCH=${BRANCH} \
  WORKSPACE=${WORKSPACE} \
  RELEASEDIR=${RELEASEDIR} \
  PUBLISHDIR=${PUBLISHDIR} \
  DOCKERIMAGE=${DOCKERIMAGE} \
  DOCKERFILE=${DOCKERFILE} \
  KEYS=${KEYS} \
  DISTNAME=${DISTNAME} \
  UPDATEPKG=${UPDATEPKG} \
  COLOR=$COLOR \
  ARCH=${ARCH} \
  WINHOST="${WINHOST}" \
  WINBLD="${WINBLD}" \
  WINREMBLD="${WINREMBLD}" \
  GIT_COMMIT="${GIT_COMMIT}" \
  INTERACTIVE="$INTERACTIVE" \
  JARS_DIR="$JARS_DIR" \
  CONFIGURE_PARAMS="$CONFIGURE_PARAMS" \
  ${SRCDIR}/deploy/platform/platform_build.sh
if [ "$?" != "0" ]
then
    RED $COLOR
    cat <<EOF >&2
============================================

Failure: The build was unsuccessful!

============================================
EOF
    NORMAL $COLOR
    exit 1
else
    GREEN $COLOR
    cat <<EOF
============================================

Success!

============================================
EOF
    NORMAL $COLOR
    if [ "$PUBLISH" = "yes" ]
    then
	touch $PUBLISHDIR/${BRANCH}_${RELEASE_VERSION}_${OS}
    fi
fi
