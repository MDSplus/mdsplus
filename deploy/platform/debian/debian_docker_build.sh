#!/bin/bash
#
# debian_docker_build is used to build, test, package and add deb's to a
# repository for debian based systems.
#
export PyLib=python2.7
config_test(){
    export MDSPLUS_DIR
    rm -Rf $(dirname "${MDSPLUS_DIR}")
    MDS_PATH=${MDSPLUS_DIR}/tdi;
    mkdir -p ${MDSPLUS_DIR};
    pushd ${MDSPLUS_DIR}/..;
    config $* --enable-debug --enable-werror;
    ln -sfT /source/xml ${MDSPLUS_DIR}/xml
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
VALGRIND_TOOLS="$(spacedelim $VALGRIND_TOOLS)"
export PYTHONDONTWRITEBYTECODE=no
export PYTHONPATH=/workspace/python
export MDS_PATH=/source/tdi
if [ -r /source/deploy/os/${OS}.env ]
then
    source /source/deploy/os/${OS}.env
fi
mkdir -p ${PYTHONPATH}
ln -sfT /source/mdsobjects/python ${PYTHONPATH}/MDSplus

### common end

config() {
    /source/configure \
        --prefix=${MDSPLUS_DIR} \
	--exec_prefix=${MDSPLUS_DIR} \
	--bindir=${MDSPLUS_DIR}/bin \
	--libdir=${MDSPLUS_DIR}/lib \
        --with-java_target=6 \
        --with-java_bootclasspath=/source/rt.jar \
        --host=${host} \
        --with-gsi=/usr:gcc$*
}
makelist(){
    dpkg -c $1 | \
    grep -v python/dist | \
    grep -v python/build | \
    grep -v egg-info | \
    grep -v '/$' | \
    awk '{for (i=6; i<NF; i++) printf $i " "; print $NF}' | \
    sort
}
debtopkg() {
    if ( echo $1 | grep mdsplus${2}_${3} >/dev/null )
    then
        echo ""
    else
        echo ${1:8+${#2}:-${#3}-${#4}-6}
    fi
}
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
  if [ "${ARCH}" = "amd64" ]
  then
    ###
    ### Build 64-bit MDSplus with debug to run regular and valgrind tests
    ###
    set -e
    MDSPLUS_DIR=/workspace/tests/64/buildroot;
    config_test 64
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
  fi
    if [ ! -z "$SANITIZE" ]
    then
	###
	### Build 64-bit versions with sanitizers and run tests with each sanitizer
	###
	for test in $(spacedelim ${SANITIZE}); do
	    echo Doing sanitize $test
	    MDSPLUS_DIR=/workspace/tests/64-san-${test}/buildroot;
	    config 64 --enable-sanitize=${test}
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
  else
    ###
    ### Build 32-bit version with debug for testing
    ###
    set -e
    MDSPLUS_DIR=/workspace/tests/32/buildroot;
    config_test 32
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
	    config_test 32 --enable-sanitize=${test}
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
    ### Build release version of MDSplus and then construct installer debs
    ###
    major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
    minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
    release=$(echo ${RELEASE_VERSION} | cut -d. -f3)
    set -e
    mkdir -p /release/${BRANCH}/DEBS/${ARCH}
    rm -Rf /workspace/releasebld/* /release/${BRANCH}/DEBS/${ARCH}/*
    if [ "${ARCH}" = "amd64" ]
    then
	MDSPLUS_DIR=/workspace/releasebld/buildroot/usr/local/mdsplus
	mkdir -p ${MDSPLUS_DIR};
	mkdir -p /workspace/releasebld/64;
	pushd /workspace/releasebld/64;
	config 64
	$MAKE
	$MAKE install
	popd;
    else
	MDSPLUS_DIR=/workspace/releasebld/buildroot/usr/local/mdsplus
	mkdir -p ${MDSPLUS_DIR};
	mkdir -p /workspace/releasebld/32;
	pushd /workspace/releasebld/32;
	config 64
	$MAKE
	$MAKE install
	popd
    fi
    BUILDROOT=/workspace/releasebld/buildroot \
	     BRANCH=${BRANCH} \
	     RELEASE_VERSION=${RELEASE_VERSION} \
	     ARCH=${ARCH} \
	     DISTNAME=${DISTNAME} \
	     PLATFORM=${PLATFORM} \
	     /source/deploy/platform/debian/debian_build_debs.py
    baddeb=0
    for deb in $(find /release/${BRANCH}/DEBS/${ARCH} -name "*\.deb")
    do
	pkg=$(debtopkg $(basename $deb) "${BNAME}" ${RELEASE_VERSION} ${ARCH})
	if [ "${#pkg}" = "0" ]
	then
	   continue
	fi
	checkfile=/source/deploy/packaging/${PLATFORM}/$pkg.${ARCH}
	if [ "$UPDATEPKG" = "yes" ]
	then
	    mkdir -p /source/deploy/packaging/${PLATFORM}
	    makelist $deb > ${checkfile}
	else
	    set +e
	    echo "Checking contents of $(basename $deb)"
	    if ( diff <(makelist $deb) <(sort ${checkfile}) )
	    then
		echo "Contents of $(basename $deb) is correct."
	    else
		RED $COLOR
		cat <<EOF >&2
=================================================================

Failure: Problem with contents of $(basename $deb)

=================================================================
EOF
		NORMAL $COLOR
		baddeb=1
	    fi
	    set -e
	fi
    done
    if [ "$baddeb" != "0" ]
    then
	RED $COLOR
	cat <<EOF >&2
=================================================================

Failure: Problem with contents of one or more debs. (see above)
         Build ABORTED

=================================================================
EOF
	NORMAL $COLOR
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
	    RED $COLOR
	    cat <<EOF >&2
=================================================================

Failure: Problem installing $deb into repository.
         Build ABORTED

=================================================================
EOF
	    NORMAL $COLOR
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
    major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
    minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
    release=$(echo ${RELEASE_VERSION} | cut -d. -f3)
    mkdir -p /publish/${BRANCH}/DEBS
    rsync -a /release/${BRANCH}/DEBS/${ARCH} /publish/${BRANCH}/DEBS/
    if [ ! -r /publish/repo ]
    then
	if ( ! rsync -a /release/repo /publish/ )
	then
	    RED $COLOR
	    cat <<EOF >&2
=================================================================

Failure: Problem copying repo into publish area.
         Build ABORTED

=================================================================
EOF
	    NORMAL $COLOR
	    exit 1
	fi
    else
	pushd /publish/repo
	reprepro clearvanished
	env HOME=/sign_keys reprepro -V --keepunused --keepunreferenced -C ${BRANCH} includedeb MDSplus ../${BRANCH}/DEBS/${ARCH}/*${major}\.${minor}\.${release}_*
	if [ "$?" != "0" ]
	then
	    RED $COLOR
	    cat <<EOF >&2
=================================================================

Failure: Problem installing debian into publish repository.
         Build ABORTED

=================================================================
EOF
	    NORMAL $COLOR
	    exit 1
	fi
	popd
    fi
fi
