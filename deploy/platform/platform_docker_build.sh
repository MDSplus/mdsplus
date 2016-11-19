#!/bin/bash
#
# platform/platform_docker_build.sh
#
# Invoked by mdsplus/deploy/platform_build.sh.
#
# Build script from within a docker image
#
runtests() {
    test64;
    test32;
    checktests;
}
testarch(){
    sanitize   $@;
    normaltest $@;
}
config() {
    /source/configure \
        --prefix=${MDSPLUS_DIR} \
        --exec_prefix=${MDSPLUS_DIR} \
        --host=$2 \
        --bindir=${MDSPLUS_DIR}/$3 \
        --libdir=${MDSPLUS_DIR}/$4 \
        --with-java_target=6 \
        --with-java_bootclasspath=/source/rt.jar \
        $5 $6 $7 $8 $9;
    status=$?
}
config_test(){
    export MDSPLUS_DIR;
    rm -Rf $(dirname "${MDSPLUS_DIR}");
    MDS_PATH=${MDSPLUS_DIR}/tdi;
    mkdir -p ${MDSPLUS_DIR};
    pushd ${MDSPLUS_DIR}/..;
    config $@ --enable-debug --enable-werror;
    cp -rf /source/xml ${MDSPLUS_DIR}/xml;
}
checkstatus(){
# checkstatus flagname "error message" $?
        if [ ! -z "$3" -a "$3" != "0" ]
        then
            RED $COLOR
            cat <<EOF >&2
======================================================

$2

======================================================
EOF
            NORMAL $COLOR
            let $1=1
        fi
}
checktests() {
    ### Check status of all tests. If errors found print error messages and then exit with failure
    failed=0;
    checkstatus failed "Failure: 64-bit test suite failed." $tests_64
    checkstatus failed "Failure: 64-bit valgrind test suite failed." $tests_valgrind64
    for test in address thread undefined; do
        checkstatus failed "Failure: 64-bit santize with ${test} failed." $(eval "\$test_64_san_${test}")
    done;
    checkstatus failed "Failure: 32-bit test suite failed." $tests_32
    checkstatus failed "Failure: 32-bit valgrind test suite failed." $tests_valgrind32
    for test in address thread undefined; do
        checkstatus failed "Failure: 32-bit santize with ${test} failed." $(eval "\$test_32_san_${test}")
    done;
    checkstatus abort "Failure: One or more tests have failed (see above). Build ABORTED" $failed
}
sanitize() {
    ### Build with sanitizers and run tests with each sanitizer
    if [ ! -z "$SANITIZE" ]
    then
        for test in $(spacedelim ${SANITIZE}); do
            echo Doing sanitize $test
            MDSPLUS_DIR=/workspace/tests/$1-san-${test}/buildroot;
            export WINEPATH=Z:\\workspace\\tests\\$1-san-${test}\\$3
            config_test $@ --enable-sanitize=${test}
            if [ "$status" = "111" ]; then
                echo "Sanitizer ${test} not supported. Skipping."
            elif [ "$status" = 0 ]; then
                $MAKE
                $MAKE install
                $MAKE -k tests 2>&1
                checkstatus test_$1_san_${test} "Failure doing $1-bit sanitize test ${test}." $?
            else
                echo "configure returned status $?"
                let test_$1_san_${test}=$status
            fi
            popd
        done
    fi
}
normaltest() {
    ### Build with debug to run regular and valgrind tests
    if [ -z "$VALGRIND_TOOLS" ] || [ ! -z "eval \${test_$1_san}" ]
    then
        set -e
        MDSPLUS_DIR=/workspace/tests/$1/buildroot;
        export WINEPATH=Z:\\workspace\\tests\\$1\\$3
        config_test $@
        $MAKE
        $MAKE install
        set +e
        ### Run standard tests
        $MAKE -k tests 2>&1
        checkstatus tests_$1 "Failure doing $1-bit normal tests." $?
        if [ ! -z "$VALGRIND_TOOLS" ]
        then
            ### Test with valgrind
            $MAKE -k tests-valgrind 2>&1
            checkstatus tests_valgrind$1 "Failure doing $1-bit valgrind tests." $?
        fi
        popd
    fi
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
export PYTHONDONTWRITEBYTECODE=no
export PyLib=$(python -V | awk '{print $2}' | awk -F. '{print "python"$1"."$2}') 
main(){
    MAKE=${MAKE:="env LANG=en_US.UTF-8 make"}
    VALGRIND_TOOLS="$(spacedelim $VALGRIND_TOOLS)"
    if [ -r /source/deploy/os/${OS}.env ]
    then
        source /source/deploy/os/${OS}.env
    fi
    abort=0
    if [ "$TEST" = "yes" ]
    then
        runtests
    fi
    if [ "${BRANCH}" = "stable" ]
    then
        BNAME=""
    else
        BNAME="-$(echo ${BRANCH} | sed -e 's/-/_/g')"
    fi
    if [ "$abort" = "0" ] && [ "$RELEASE" = "yes" ]
    then
        buildrelease
    fi
    if [ "$abort" = "0" ] && [ "$PUBLISH" = "yes" ]
    then
        publish
    fi
}
if [ "$(basename $0)" = "platform_docker_build.sh" ]
then
    . /source/deploy/platform/${PLATFORM}/${PLATFORM}_docker_build.sh
    main
else
    . /source/deploy/platform/${PLATFORM}/${PLATFORM}_docker_build.sh
fi

