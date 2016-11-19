#!/bin/bash
#
#
# redhat_docker_build.sh is used to build, test, package and add rpms's to a
# repository for redhat based systems.
#
#
if [ -r /source/deploy/os/${OS}.env ]
then
    source /source/deploy/os/${OS}.env
fi
config() {
    /source/configure  \
	--prefix=${MDSPLUS_DIR} \
	--exec_prefix=${MDSPLUS_DIR} \
	--bindir=${MDSPLUS_DIR}/bin$1 \
	--libdir=${MDSPLUS_DIR}/lib$1 \
	--with-gsi=/usr:gcc$2 \
	--with-java_target=6 \
	--with-java_bootclasspath=/source/rt.jar \
	--host=$3-linux $4 $5;
}
makelist(){
    rpm2cpio $1 | \
	cpio --list --quiet | \
	grep -v python/dist | \
	grep -v python/build | \
	grep -v egg-info | \
	sort
}
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

MAKE=${MAKE:="env LANG=en_US.UTF-8 make"}
VALGRIND_TOOLS="$(spacedelim ${VALGRIND_TOOLS})"
export PYTHONDONTWRITEBYTECODE=no

set -e
if [ "$TEST" = "yes" ]
then
    ###
    ### Clean up workspace
    ###
    rm -Rf /workspace/tests
    ###
    ### Define PyLib for use in tests which use tdi python functions
    ###
    export PyLib=$(python -V | awk '{print $2}' | awk -F. '{print "python"$1"."$2}')
    ###
    ### Build 64-bit MDSplus with debug to run regular and valgrind tests
    ###
    MDSPLUS_DIR=/workspace/tests/64/buildroot;
    mkdir -p ${MDSPLUS_DIR};
    MDS_PATH=${MDSPLUS_DIR}/tdi;
    pushd /workspace/tests/64;
    config 64 64 x86_64 --enable-debug --enable-werror
    $MAKE
    $MAKE install
    ###
    ### Run regular and valgrind tests
    ###
    set +e
    if [ ! -z "$VALGRIND_TOOLS" ]
    then
	###
	### Test with valgrind
	###
	if ( ! $MAKE -k tests-valgrind 2>&1 )
	then
	    RED $COLOR
	    cat <<EOF >&2
=======================================================

Failure doing 64-bit valgrind tests.

=======================================================
EOF
	    NORMAL $COLOR
	    tests_valgrind64=1
	fi
	$MAKE clean_TESTS
    fi
    ###
    ### Run standard tests
    ###
    if ( ! $MAKE -k tests 2>&1 )
    then
	RED $COLOR
	cat <<EOF >&2
======================================================

Failure doing 64-bit normal tests.

======================================================
EOF
	NORMAL $COLOR
	tests_64=1
    fi
    popd
    if [ ! -z "$SANITIZE" ]
    then
	###
	### Build 64-bit versions with sanitizers and run tests with each sanitizer
	###
	for test in $(spacedelim ${SANITIZE}); do
	    echo Doing sanitize $test
	    MDSPLUS_DIR=/workspace/tests/64-san-${test}/buildroot;
	    MDS_PATH=${MDSPLUS_DIR}/tdi;
	    mkdir -p ${MDSPLUS_DIR};
	    pushd /workspace/tests/64-san-${test};
	    config 64 64 x86_64 --enable-debug --enable-sanitize=${test}
	    status=$?
	    if [ "$status" = "111" ]; then
		echo "Sanitizer ${test} not supported. Skipping."
		continue
	    elif [ "$status" = 0 ]; then
		$MAKE
		$MAKE install
		if ( ! $MAKE -k tests 2>&1 )
		then
		    RED $COLOR
		    cat <<EOF >&2
=======================================================

Failure doing 64-bit sanitize test ${test}

=======================================================
EOF
		    NORMAL $COLOR
		    let test_64_san_${test}=1
		fi
	    else
		echo "configure returned status $?"
		let test_64_san_${test}=$status
	    fi
	    popd
	done
    fi
    ###
    ### Build 32-bit version with debug for testing
    ###
    set -e
    MDSPLUS_DIR=/workspace/tests/32/buildroot;
    MDS_PATH=${MDSPLUS_DIR}/tdi;
    mkdir -p ${MDSPLUS_DIR};
    pushd /workspace/tests/32;
    config 32 32 i686 --enable-debug --enable-werror
    $MAKE
    $MAKE install
    set +e;
    if [ ! -z "$VALGRIND_TOOLS" ]
    then
	###
	### Test with valgrind
	###
	if ( ! $MAKE -k tests-valgrind 2>&1 )
	then
	    RED $COLOR
	    cat <<EOF 2>&1
===================================================

Failure during 32-bit valgrind testing.

===================================================
EOF
	    NORMAL $COLOR
	    tests_valgrind32=1
	fi
	$MAKE clean_TESTS
    fi
    ###
    ### Run standard tests on 32-bit
    ###
    if ( ! $MAKE -k tests 2>&1 )
    then
	RED $COLOR
	cat <<EOF >&2
===================================================

Failure during 32-bit normal testing.

===================================================
EOF
	NORMAL $COLOR
	tests_32=1
    fi
    popd;
    if [ ! -z "$SANITIZE" ]
    then
	###
	### Build and test with sanitizers
	###
	for test in $(spacedelim ${SANITIZE}); do
	    if [ "$test" = "thread" ]; then
		echo "No 32-bit support for sanitize=thread. Skipping."
		break
	    fi
	    MDSPLUS_DIR=/workspace/tests/32-san-${test}/buildroot;
	    MDS_PATH=${MDSPLUS_DIR}/tdi;
	    mkdir -p ${MDSPLUS_DIR};
	    pushd /workspace/tests/32-san-${test};
	    config 32 32 i686 --enable-debug --enable-sanitize=${test}
	    status=$?
	    if [ "$status" == 111 ]; then
		echo "Sanitizer ${test} not supported. Skipping."
		break
	    elif [ "$status" = 0 ]; then
		$MAKE
		$MAKE install
		if ( ! $MAKE -k tests 2>&1 )
		then
		    RED $COLOR
		    cat <<EOF >&2
=======================================================

Failure during 32-bit sanitize ${test} testing.

=======================================================
EOF
		    NORMAL $COLOR
		    let test_32_san_${test}=1
		fi
	    else
		echo "Configure returned a status 0f $status"
		let test_32_san_${test}=$status
	    fi
	    popd
	done
    fi;
    ###
    ### Check status of all tests. If errors found print error messages and then exit with failure
    ###
    failed=0;
    if [ ! -z "$tests_64" -a "$tests_64" != "0" ]
    then
	RED $COLOR
	cat <<EOF >&2
=================================================================

Failure: 64-bit test suite failed

=================================================================
EOF
	NORMAL $COLOR
	failed=1;
    fi;
    if [ ! -z "$tests_valgrind64" -a "$tests_valgrind64" != "0" ]
    then
	RED $COLOR
	cat <<EOF >&2
=================================================================

Failure: 64-bit valgrind test suite failed

=================================================================
EOF
	NORMAL $COLOR
	failed=1;
    fi
    for test in address thread undefined; do
	eval "status=\$test_64_san_${test}"
	if [ ! -z "$status" -a "$status" != "0" ]
	then
	    RED $COLOR
	    cat <<EOF >&2
=================================================================

Failure: 64-bit santize with ${test} failed

=================================================================
EOF
	    NORMAL $COLOR
	    failed=1;
	fi
    done;
    if [ ! -z "$tests_32" -a "$tests_32" != "0" ]
    then
	RED $COLOR
	cat <<EOF >&2
=================================================================

Failure: 32-bit test suite failed

=================================================================
EOF
	NORMAL $COLOR
	failed=1;
    fi;
    if [ ! -z "$tests_valgrind32" -a "$tests_valgrind32" != "0" ]
    then
	RED $COLOR
	cat <<EOF >&2
=================================================================

Failure: 32-bit valgrind test suite failed

=================================================================
EOF
	NORMAL $COLOR
	failed=1;
    fi;
    for test in address thread undefined; do
	eval "status=\$test_32_san_${test}"
	if [ ! -z "$status" -a "$status" != "0" ]
	then
	    RED $COLOR
	    cat <<EOF >&2
=================================================================

Failure: 32-bit santize with ${test} failed

=================================================================
EOF
	    NORMAL $COLOR
	    failed=1
	fi
    done
    if [ "$failed" = "1" ]
    then
	RED $COLOR
	cat <<EOF >&2
=================================================================

Failure: One or more tests have failed (see above). Build ABORTED 

=================================================================
EOF
	NORMAL $COLOR
	exit 1
    fi
fi
if [ "${BRANCH}" = "stable" ]
then
    BNAME=""
else
    BNAME="-$(echo ${BRANCH} | sed -e 's/-/_/g')"
fi
if [ "$RELEASE" = "yes" ]
then
    ###
    ### Clean up workspace
    ###
    rm -Rf /workspace/releasebld
    ###
    ### Build release version of MDSplus and then construct installer rpms
    ###
    set -e
    MDSPLUS_DIR=/workspace/releasebld/buildroot/usr/local/mdsplus
    mkdir -p ${MDSPLUS_DIR};
    mkdir -p /workspace/releasebld/64;
    pushd /workspace/releasebld/64;
    config 64 64 x86_64
    $MAKE
    $MAKE install
    popd;
    MDSPLUS_DIR=/workspace/releasebld/buildroot/usr/local/mdsplus
    mkdir -p ${MDSPLUS_DIR};
    mkdir -p /workspace/releasebld/32;
    pushd /workspace/releasebld/32;
    config 32 64 i686
    $MAKE
    $MAKE install
    popd
    BUILDROOT=/workspace/releasebld/buildroot
    echo "Building rpms";
    ###
    ### Setup repository rpm info
    ###
    mkdir -p ${BUILDROOT}/etc/yum.repos.d;
    mkdir -p ${BUILDROOT}/etc/pki/rpm-gpg/;
    mkdir -p /release/RPMS;
    cp /source/deploy/platform/redhat/RPM-GPG-KEY-MDSplus ${BUILDROOT}/etc/pki/rpm-gpg/;
    if [ -d /sign_keys/.gnupg ]
    then
	GPGCHECK="1"
    else
	echo "WARNING: Signing Keys Unavailable. Building unsigned RPMS"
	GPGCHECK="0"
    fi
    cat - > ${BUILDROOT}/etc/yum.repos.d/mdsplus${BNAME}.repo <<EOF
[MDSplus${BNAME}]
name=MDSplus${BNAME}
baseurl=http://www.mdsplus.org/dist/${OS}/${BRANCH}/RPMS
enabled=1
gpgcheck=${GPGCHECK}
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-MDSplus
metadata_expire=300
EOF
    ###
    ### Build RPMS
    ###
    ### Clean up release stage area
    ###
    rm -Rf /release/*
    
    BRANCH=${BRANCH} \
	  RELEASE_VERSION=${RELEASE_VERSION} \
	  BNAME=${BNAME} \
	  DISTNAME=${DISTNAME} \
	  BUILDROOT=${BUILDROOT} \
	  PLATFORM=${PLATFORM} \
	  /source/deploy/platform/${PLATFORM}/${PLATFORM}_build_rpms.py;
    createrepo -q /release/RPMS
    badrpm=0
    for rpm in $(find /release/RPMS -name '*\.rpm')
    do
	pkg=$(echo $(basename $rpm) | cut -f3 -d-)
	#
	# Skip main installer which only has package dependencies and no files
	# Skip the repo rpm which will contain the branch name
	#
	if ( echo ${pkg} | grep '\.' >/dev/null ) || ( echo ${pkg} | grep repo >/dev/null )
	then
	    continue
	fi
	pkg=${pkg}.$(echo $(basename $rpm) | cut -f5 -d- | cut -f3 -d.)
	checkfile=/source/deploy/packaging/${PLATFORM}/$pkg
	if [ "$UPDATEPKG" = "yes" ]
	then
	    mkdir -p /source/deploy/packaging/${PLATFORM}/
	    makelist $rpm > ${checkfile}
	else
	    set +e
	    echo "Checking contents of $(basename $rpm)"
	    if ( diff <(makelist $rpm) <(sort ${checkfile}) )
	    then
		echo "Contents of $(basename $rpm) is correct."
	    else
		RED $COLOR
		cat <<EOF >&2
=================================================================

Failure: Problem with contents of $(basename $rpm)

=================================================================
EOF
		NORMAL $COLOR
		badrpm=1
	    fi
	    set -e
	fi
    done
    if [ "$badrpm" != "0" ]
    then
	RED $COLOR
	cat <<EOF >&2
=================================================================

Failure: Problem with contents of one or more rpms. (see above)
         Build ABORTED

=================================================================
EOF
	NORMAL $COLOR
	exit 1
    fi
fi

if [ "$PUBLISH" = "yes" ]
then
    ###
    ### DO NOT CLEAN /publish as it may contain valid older release rpms
    ###
    if ( ! rsync -a --exclude=repodata /release/RPMS /publish/ )
    then
	RED $COLOR
	cat <<EOF >&2
=================================================================

Failure: Problem copying release rpms to publish area!
         Build ABORTED

=================================================================
EOF
	NORMAL $COLOR
       	exit 1
    fi
    if ( createrepo -h | grep '\-\-deltas' > /dev/null )
    then
	use_deltas="--deltas"
    fi
    if ( ! createrepo -q --update --cachedir /publish/cache ${use_deltas} /publish/RPMS )
    then
	RED $COLOR
	cat <<EOF >&2
=================================================================

Failure: Problem creating rpm repository in publish area!
         Build ABORTED

=================================================================
EOF
	NORMAL $COLOR
       	exit 1
    fi
fi
