#!/bin/bash
#
# platform/platform_docker_build.sh
#
# Invoked by mdsplus/deploy/platform_build.sh.
#
# Build script from within a docker image
#
export HOME=/tmp/home
mkdir -p $HOME
tio(){
    :&& /source/deploy/platform/timeout.sh "$@";
    return $?;
}
getenv() {
    eval "echo \$$1"
}
runtests() {
    # run tests with the platform specific params read from test32 and test64
    testarch ${test64};
    testarch ${test32};
    if [ -z "$NOMAKE" ]; then
        echo "TEST RESULTS:"
        checktests;
        GREEN $COLOR
        echo "SUCCESS"
        NORMAL $COLOR
    fi
}
testarch(){
    sanitize   $@;
    normaltest $@;
}
config() {
    :&& /source/configure \
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
    export WINEPATH="Z:${MDSPLUS_DIR}/$3"
    rm -Rf $(dirname "${MDSPLUS_DIR}");
    mkdir -p ${MDSPLUS_DIR};
    cp -rf /source/xml ${MDSPLUS_DIR}/xml;
    MDS_PATH=${MDSPLUS_DIR}/tdi;
    pushd ${MDSPLUS_DIR}/..;
    config $@ --enable-debug --enable-werror;
}
checkstatus(){
# checkstatus flagname "error message" $?
        if [ ! -z "$3" -a "$3" != "0" ]
        then
            RED $COLOR
            if [ "$1" = "abort" ]
            then
                ABORT="                                         Build ABORTED"
            fi
            cat <<EOF >&2
======================================================

$2
$ABORT
======================================================
EOF
            NORMAL $COLOR
            if [ "$1" = "abort" ]
            then
                exit $3
            else
                let $1=$3
                return $3
            fi
        fi
}
checktests() {
    set +e
    ### Check status of all tests. If errors found print error messages and then exit with failure
    checkstatus failed "Failure: 64-bit test suite failed." $tests_64
    checkstatus failed "Failure: 64-bit valgrind test suite failed." $tests_64_val
    for test in address thread undefined; do
        checkstatus failed "Failure: 64-bit santize with ${test} failed." $(getenv "tests_64_san_${test}")
    done;
    checkstatus failed "Failure: 32-bit test suite failed." $tests_32
    checkstatus failed "Failure: 32-bit valgrind test suite failed." $tests_32_val
    for test in address thread undefined; do
        checkstatus failed "Failure: 32-bit santize with ${test} failed." $(getenv "tests_32_san_${test}")
    done;
    checkstatus abort "Failure: One or more tests have failed (see above)." $failed
}
sanitize() {
    ### Build with sanitizers and run tests with each sanitizer
    SANITIZE="$(spacedelim $SANITIZE)"
    if [ ! -z "$SANITIZE" ]
    then
        for test in ${SANITIZE}; do
            echo Doing sanitize ${test}
            MDSPLUS_DIR=/workspace/tests/${1}-san-${test}/buildroot;
            config_test $@ --enable-sanitize=${test}
            if [ "$status" = "111" ]; then
                echo "Sanitizer ${test} not supported. Skipping."
            elif [ "$status" = "0" ]; then
              if [ -z "$NOMAKE" ]; then
                $MAKE
                $MAKE install
                :&& tio 1800 $MAKE -k tests 2>&1
                checkstatus tests_${1}_san_${test} "Failure doing $1-bit sanitize test ${test}." $?
              fi
            else
                echo "configure returned status $?"
                let tests_${1}_san_${test}=$status
            fi
            popd
        done
    fi
}
normaltest() {
    gettimeout() {
        declare -i n=1800*$#
        echo $n
    }
    ### Build with debug to run regular and valgrind tests
    VALGRIND_TOOLS="$(spacedelim $VALGRIND_TOOLS)"
    MDSPLUS_DIR=/workspace/tests/$1/buildroot;
    config_test $@
   if [ -z "$NOMAKE" ]; then
    $MAKE
    $MAKE install
    ### Run standard tests
    :&& tio 600 $MAKE -k tests 2>&1
    checkstatus tests_$1 "Failure doing $1-bit normal tests." $?
    if [ ! -z "$VALGRIND_TOOLS" ]
    then
        ### Test with valgrind
        to=$( gettimeout $VALGRIND_TOOLS )
        :&& tio $to  $MAKE -k tests-valgrind 2>&1
        checkstatus tests_${1}_val "Failure doing $1-bit valgrind tests." $?
    fi
   fi
    popd
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
    if [ -r /source/deploy/os/${OS}.env ]
    then
        source /source/deploy/os/${OS}.env
    fi
    if [ "$TEST" = "yes" ]
    then
        set +e
        runtests
    fi
    if [ "${BRANCH}" = "stable" ]
    then
        BNAME=""
    else
        BNAME="-$(echo ${BRANCH} | sed -e 's/-/_/g')"
    fi
    if [ "$RELEASE" = "yes" ]
    then
        set +e
        buildrelease
    fi
    if [ "$PUBLISH" = "yes" ]
    then
        set +e
        publish
    fi
}
source /source/deploy/platform/${PLATFORM}/${PLATFORM}_docker_build.sh
if [ ! -z "$0" ] && [ ${0:0:1} != "-" ] && [ "$( basename $0 )" = "platform_docker_build.sh" ]
then
    main
fi
