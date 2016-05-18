#!/bin/bash
#
#
# debian_docker_build is used to build, test, package and add deb's to a
# repository for debian based systems.
#
if [ -r /source/deploy/os/${OS}.env ]
then
    source /source/deploy/os/${OS}.env
fi
printenv
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

MAKE=${MAKE:="env LANG=en_US.UTF-8 make"}
VALGRIND_TOOLS="$(spacedelim $VALGRIND_TOOLS)"

set -e
if [ "$ARCH" = "amd64" ]
then
    if [ -z "$host" ]
    then
	host=x86_64-linux
    fi
else
    if [ -z "$host" ]
    then
	host=i686-linux
    fi
fi

if [ "$TEST" = "yes" ]
then
    ###
    ### Clean up workspace
    ###
    rm -Rf /workspace/tests
    if [ "${ARCH}" = "amd64" ]
    then
	###
	### Build 64-bit MDSplus with debug to run regular and valgrind tests
	###
	MDSPLUS_DIR=/workspace/tests/64/buildroot;
	mkdir -p ${MDSPLUS_DIR};
	MDS_PATH=${MDSPLUS_DIR}/tdi;
	pushd /workspace/tests/64;
	/source/configure  \
	    --prefix=${MDSPLUS_DIR} \
	    --exec_prefix=${MDSPLUS_DIR} \
	    --bindir=${MDSPLUS_DIR}/bin \
	    --libdir=${MDSPLUS_DIR}/lib \
	    --with-gsi=/usr:gcc64 \
	    --with-java_target=6 \
	    --with-java_bootclasspath=/source/rt.jar \
	    --enable-debug --host=${host};
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
	    $MAKE -k tests-valgrind 2>&1;
	    tests_valgrind64=$?
	    $MAKE clean_TESTS
	fi
	###
	### Run standard tests
	###
	$MAKE -k tests 2>&1;
	tests_64=$?
	popd;
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
		/source/configure \
		    --prefix=${MDSPLUS_DIR} \
		    --exec_prefix=${MDSPLUS_DIR} \
		    --bindir=${MDSPLUS_DIR}/bin \
		    --libdir=${MDSPLUS_DIR}/lib \
		    --with-gsi=/usr:gcc64 \
		    --with-java_target=6 \
		    --with-java_bootclasspath=/source/rt.jar \
		    --enable-debug --host=${host} \
		    --enable-sanitize=${test}
		status=$?
		if [ "$status" = "111" ]; then
		    echo "Sanitizer ${test} not supported. Skipping."
		    continue
		elif [ "$status" = 0 ]; then
		    $MAKE
		    $MAKE install
		    $MAKE -k tests 2>&1;
		    let test_64_san_${test}=$?
		else
		    echo "configure returned status $status"
		    let test_64_san_${test}=$status
		fi
		popd
	    done
	fi
    else
	###
	### Build 32-bit version with debug for testing
	###
	set -e
	MDSPLUS_DIR=/workspace/tests/32/buildroot;
	MDS_PATH=${MDSPLUS_DIR}/tdi;
	mkdir -p ${MDSPLUS_DIR};
	pushd /workspace/tests/32;
	/source/configure \
	    --prefix=${MDSPLUS_DIR} \
	    --exec_prefix=${MDSPLUS_DIR} \
	    --bindir=${MDSPLUS_DIR}/bin \
	    --libdir=${MDSPLUS_DIR}/lib \
	    --with-gsi=/usr:gcc32 \
	    --with-java_target=6 \
	    --with-java_bootclasspath=/source/rt.jar \
	    --enable-debug --host=${host};
	$MAKE
	$MAKE install
	set +e;
	if [ ! -z "$VALGRIND_TOOLS" ]
	then
	    ###
	    ### Test with valgrind
	    ###
	    $MAKE -k tests-valgrind 2>&1;
	    tests_valgrind32=$?;
	    $MAKE clean_TESTS
	fi
	###
	### Run standard tests on 32-bit
	###
	$MAKE -k tests 2>&1;
	tests_32=$?;
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
		/source/configure \
		    --prefix=${MDSPLUS_DIR} \
		    --exec_prefix=${MDSPLUS_DIR} \
		    --bindir=${MDSPLUS_DIR}/bin \
		    --libdir=${MDSPLUS_DIR}/lib \
		    --with-gsi=/usr:gcc32 \
		    --with-java_target=6 \
		    --with-java_bootclasspath=/source/rt.jar \
		    --enable-debug --host=${host} \
		    --enable-sanitize=${test}
		status=$?
		if [ "$status" == 111 ]; then
		    echo "Sanitizer ${test} not supported. Skipping."
		    break
		elif [ "$status" = 0 ]; then
		    $MAKE
		    $MAKE install
		    $MAKE -k tests 2>&1;
		    let test_32_san_${test}=$?
		else
		    echo "Configure returned a status 0f $status"
		    let test_32_san_${test}=$status
		fi
		popd
	    done
	fi;
    fi
    ###
    ### Check status of all tests. If errors found print error messages and then exit with failure
    ###
    echo "Checking test results"
    failed=0;
    if [ ! -z "$tests_64" -a "$tests_64" != "0" ]
    then
	cat <<EOF >&2
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Failure: 64-bit test suite failed

|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
EOF
	failed=1;
    fi;
    if [ ! -z "$tests_valgrind64" -a "$tests_valgrind64" != "0" ]
    then
	cat <<EOF >&2
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Failure: 64-bit valgrind test suite failed

|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
EOF
	failed=1;
    fi;
    for test in address thread undefined; do
	eval "status=\$test_64_san_${test}"
	if [ ! -z "$status" -a "$status" != "0" ]
	then
	    cat <<EOF >&2
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Failure: 64-bit santize with ${test} failed

|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
EOF
	    failed=1;
	fi
    done;
    if [ ! -z "$tests_32" -a "$tests_32" != "0" ]
    then
	cat <<EOF >&2
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Failure: 32-bit test suite failed

|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
EOF
	failed=1;
    fi;
    if [ ! -z "$tests_valgrind32" -a "$tests_valgrind32" != "0" ]
    then
	cat <<EOF >&2
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Failure: 32-bit valgrind test suite failed

|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
EOF
	failed=1;
    fi;
    for test in address thread undefined; do
	eval "status=\$test_32_san_${test}"
	if [ ! -z "$status" -a "$status" != "0" ]
	then
	    cat <<EOF >&2
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Failure: 32-bit santize with ${test} failed

|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
EOF
	    failed=1;
	fi
    done;
    if [ "$failed" = "1" ]
    then
	cat <<EOF >&2
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Failure: One or more tests have failed (see above). Build ABORTED 

|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
EOF
	exit 1;
    fi;
    echo "All tests succeeded"
fi

if [ "${BRANCH}" = "stable" ]
then
    BNAME=""
else
    BNAME="-${BRANCH}"
fi

debtopkg() {
    if ( echo $1 | grep mdsplus${2}_${3} >/dev/null )
    then
        echo ""
    else
        echo ${1:8+${#2}:-${#3}-${#4}-6}
    fi
}

if [ "$RELEASE" = "yes" ]
then
    ###
    ### Build release version of MDSplus and then construct installer debs
    ###
    major=$(echo ${VERSION} | cut -d. -f1)
    minor=$(echo ${VERSION} | cut -d. -f2)
    release=$(echo ${VERSION} | cut -d. -f3)
    set -e
    rm -Rf /workspace/releasebld/* /release/*
    if [ "${ARCH}" = "amd64" ]
    then
	MDSPLUS_DIR=/workspace/releasebld/buildroot/usr/local/mdsplus
	mkdir -p ${MDSPLUS_DIR};
	mkdir -p /workspace/releasebld/64;
	pushd /workspace/releasebld/64;
	/source/configure \
	    --prefix=${MDSPLUS_DIR} \
	    --exec_prefix=${MDSPLUS_DIR} \
	    --bindir=${MDSPLUS_DIR}/bin \
	    --libdir=${MDSPLUS_DIR}/lib \
	    --with-gsi=/usr:gcc64 \
	    --with-java_target=6 \
	    --with-java_bootclasspath=/source/rt.jar \
	    --host=${host};
	$MAKE
	$MAKE install
	popd;
    else
	MDSPLUS_DIR=/workspace/releasebld/buildroot/usr/local/mdsplus
	mkdir -p ${MDSPLUS_DIR};
	mkdir -p /workspace/releasebld/32;
	pushd /workspace/releasebld/32;
	/source/configure  \
	    --prefix=${MDSPLUS_DIR} \
	    --exec_prefix=${MDSPLUS_DIR} \
	    --bindir=${MDSPLUS_DIR}/bin \
	    --libdir=${MDSPLUS_DIR}/lib \
	    --with-gsi=/usr:gcc64 \
	    --with-java_target=6 \
	    --with-java_bootclasspath=/source/rt.jar \
	    --host=${host};
	$MAKE
	$MAKE install
	popd
    fi
    mkdir -p /release/${BRANCH}/DEBS/${ARCH}
    BUILDROOT=/workspace/releasebld/buildroot \
	     BRANCH=${BRANCH} \
	     VERSION=${VERSION} \
	     ARCH=${ARCH} \
	     DISTNAME=${DISTNAME} \
	     PLATFORM=${PLATFORM} \
	     /source/deploy/platform/debian/debian_build_debs.py
    baddeb=0
    for deb in $(find /release/${BRANCH}/DEBS/${ARCH} -name "*\.deb")
    do
	pkg=$(debtopkg $(basename $deb) ${BNAME} ${VERSION} ${ARCH})
	if [ "${#pkg}" = "0" ]
	then
	   continue
	fi
	checkfile=/source/deploy/packaging/${PLATFORM}/$pkg.${ARCH}
	if [ "$UPDATEPKG" = "yes" ]
	then
	    mkdir -p /source/deploy/packaging/${PLATFORM}
	    dpkg -c $deb | \
		grep -v python/dist | \
		grep -v python/build | \
		grep -v egg-info | \
		awk '{print $6}' > ${checkfile}
	else
	    set +e
	    echo "Checking contents of $(basename $deb)"
	    if ( dpkg -c $deb | \
		grep -v python/dist | \
		grep -v python/build | \
		grep -v egg-info | \
		awk '{print $6}' | diff - ${checkfile} )
	    then
		echo "Contents of $(basename $deb) is correct."
	    else
		cat <<EOF >&2
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Failure: Problem with contents of $(basename $deb)

|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
EOF
		baddeb=1
	    fi
	    set -e
	fi
    done
    if [ "$baddeb" != "0" ]
    then
	cat <<EOF >&2
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Failure: Problem with contents of one or more debs. (see above)
         Build ABORTED

|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
EOF
	exit 1
    fi
    echo "Building repo";
    mkdir -p /release/repo/conf
    mkdir -p /release/repo/db
    mkdir -p /release/repo/dists
    mkdir -p /release/repo/pool
    if [ "${BRANCH}" = "alpha" -o "${BRANCH}" = "stable" ]
    then
	component=""
    else
	component=" ${BRANCH}"
    fi
    arches=$(spacedelim ${ARCHES})
    cat - <<EOF > /release/repo/conf/distributions
Origin: MDSplus Development Team
Label: MDSplus
Codename: MDSplus
Architectures: ${arches}
Components: alpha stable${component}
Description: MDSplus packages
EOF
    if [ -d /sign_keys/.gnupg ]
    then
	ls -l /sign_keys/.gnupg
	echo "SignWith: MDSplus" >> /release/repo/conf/distributions
    fi
    export HOME=/sign_keys
    pushd /release/repo
    reprepro clearvanished
    for deb in $(find /release/${BRANCH}/DEBS/${ARCH} -name "*${major}\.${minor}\.${release}_*")
    do
	if ( ! reprepro -V -C ${BRANCH} includedeb MDSplus $deb )
	then
	    cat <<EOF >&2
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Failure: Problem installing $deb into repository.
         Build ABORTED

|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
EOF
	    exit 1
	fi
	
    done
    popd
fi

if [ "$PUBLISH" = "yes" ]
then
    ###
    ### DO NOT CLEAN /publish as it may contain valid older release packages
    ###
    mkdir -p /publish/${BRANCH}/DEBS
    rsync -a /release/${BRANCH}/DEBS/${ARCH} /publish/${BRANCH}/DEBS/
    if [ ! -r /publish/repo ]
    then
	if ( ! rsync -a /release/repo /publish/ )
	then
	    cat <<EOF >&2
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Failure: Problem copying repo into publish area.
         Build ABORTED

|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
EOF
	    exit 1
	fi
    else
	pushd /publish/repo
	reprepro clearvanished
	for deb in $(find /release/${BRANCH}/DEBS/${ARCH} -name "*${major}\.${minor}\.${release}_*")
	do
	    if ( ! reprepro -V -C ${BRANCH} includedeb MDSplus $deb )
	    then
		cat <<EOF >&2
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Failure: Problem installing debian into publish repository.
         Build ABORTED

|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
EOF
		exit 1
	    fi
	done
	popd
    fi
fi
