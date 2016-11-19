#!/bin/bash
#
# windows_docker_build.sh - build windows installer
#
export PyLib=python27
##
### common begin
##
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
##
### common end
##
config() {
    /source/configure  \
        --prefix=${MDSPLUS_DIR} \
        --exec_prefix=${MDSPLUS_DIR} \
        --bindir=${MDSPLUS_DIR}/bin_$1 \
        --libdir=${MDSPLUS_DIR}/bin_$1 \
        --with-java_target=6 \
        --with-java_bootclasspath=/source/rt.jar \
        --host=$2-w64-mingw32 $3 $4 $5 $6 $7 $8 $9;
}
export JNI_INCLUDE_DIR=/source/3rd-party-apis/windows-jdk
export JNI_MD_INCLUDE_DIR=/source/3rd-party-apis/windows-jdk/win32
if [ "$TEST" = "yes" ]
then
    ###
    ### Build 64-bit MDSplus with debug to run regular and valgrind tests
    ###
    set -e
    MDSPLUS_DIR=/workspace/tests/64/buildroot;
    export WINEPATH=Z:\\workspace\\tests\\64\\bin_x86_64
    config_test x86_64 x86_64
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
            export WINEPATH=Z:\\workspace\\tests\\64-san-${test}\\bin_x86_64
            config_test x86_64 x86_64 --enable-sanitize=${test}
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
    export WINEPATH=Z:\\workspace\\tests\\32\\bin_x86
    config_test x86 i686
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
            export WINEPATH=Z:\\workspace\\tests\\32-san-${test}\\bin_x86
            config_test x86 i686 --enable-sanitize=${test}
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
    MDSPLUS_DIR=/workspace/releasebld/buildroot
    mkdir -p ${MDSPLUS_DIR};
    mkdir -p /workspace/releasebld/64;
    pushd /workspace/releasebld/64;
    config x86_64 x86_64
    $MAKE
    $MAKE install
    popd;
    mkdir -p /workspace/releasebld/32;
    pushd /workspace/releasebld/32;
    config x86 i686
    $MAKE
    $MAKE install
    popd
    ###
    ### pack installer
    ###
    major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
    minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
    release=$(echo ${RELEASE_VERSION} | cut -d. -f3)
    if [ "${BRANCH}" = "stable" ]
    then
        bname=""
    else
        bname="-${BRANCH}"
    fi
    if [ ! -z "$WINHOST" -a -r /winbld -a ! -z "$WINREMBLD" ]
    then
        windows_cleanup() {
            rm -Rf /winbld/${tmpdir}
        }
        pushd /winbld
        tmpdir=$(mktemp -d mdsplus-XXXXXXXXXX)
        trap windows_cleanup EXIT
        topsrcdir=${WINREMBLD}/${tmpdir}
        cd ${tmpdir}
        rsync -am --include="*/" --include="*.h*" --include="*.def" --exclude="*" /source/ ./
        rsync -am /workspace/releasebld/64/include/config.h ./include/
        rsync -a /source/mdsobjects/cpp /source/mdsobjects/MdsObjects* /source/mdsobjects/VS-* ./mdsobjects/
        rsync -a /source/deploy/platform/windows/winbld.bat ./deploy/
        rsync -a ${MDSPLUS_DIR}/bin_* ./
        curl http://${WINHOST}:8080${topsrcdir}/deploy/winbld.bat
        # see if files are there
        if ( ls /winbld/${tmpdir}/bin_x86*/*.lib /winbld/${tmpdir}/bin_x86*/MdsObjectsCppShr-VS.* > /dev/null )
        then
            rsync -av --include="*/" --include="*.lib" --include="MdsObjectsCppShr-VS.dll" --exclude="*" /winbld/${tmpdir}/bin_x86* ${MDSPLUS_DIR}/
        else
            RED $COLOR
            cat <<EOF >&2
============================================

Failure: Problem building Visual Studio dll

============================================
EOF
            NORMAL $COLOR
            exit 1
        fi
        vs="-DVisualStudio"
        popd
    fi
    pushd $MDSPLUS_DIR
    makensis -DMAJOR=${major} -DMINOR=${minor} -DRELEASE=${release} -DFLAVOR=${bname} -NOCD \
             -DOUTDIR=/release ${vs} /source/deploy/packaging/${PLATFORM}/mdsplus.nsi
    popd
    if [ -d /sign_keys ]
    then
        set +e
        for timestamp_server in http://timestamp.comodoca.com/authenticode http://timestamp.verisign.com/scripts/timestamp.dll http://timestamp.globalsign.com/scripts/timestamp.dll http://tsa.starfieldtech.com
        do
            if ( signcode -spc /sign_keys/mdsplus.spc \
                                -v /sign_keys/mdsplus.pvk \
                                -a sha1 \
                                -$ individual \
                                -n MDSplus  \
                                -i http://www.mdsplus.org/ \
                                -t http://timestamp.verisign.com/scripts/timestamp.dll \
                                -tr 10 /release/MDSplus${bname}-${major}.${minor}-${release}.exe <<EOF
mdsplus
EOF
               )
            then
                break
            fi
        done
        set -e
    fi
fi

if [ "$PUBLISH" = "yes" ]
then
    major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
    minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
    release=$(echo ${RELEASE_VERSION} | cut -d. -f3)
    if [ "${BRANCH}" = "stable" ]
    then
        bname=""
    else
        bname="-${BRANCH}"
    fi
    rsync -a /release/MDSplus${bname}-${major}.${minor}-${release}.exe /publish/${BRANCH}/
fi

