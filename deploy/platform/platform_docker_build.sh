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
    checktests;
}
testarch(){
    archlist="${archlist} $1"
    echo archlist=${archlist}
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

checktestarch() {
    set +e
    ### Check status of all tests. If errors found print error messages and then exit with failure
    checkstatus failed "Failure: ${1}-bit make failed."                $(getenv    "make_${1}")
    checkstatus failed "Failure: ${1}-bit install failed."             $(getenv "install_${1}")
    checkstatus failed "Failure: ${1}-bit test suite failed."          $(getenv   "tests_${1}")
    checkstatus failed "Failure: ${1}-bit valgrind test suite failed." $(getenv   "tests_${1}_val")
    for test in address thread undefined; do
        checkstatus failed "Failure: ${1}-bit santize with ${test} make failed."    $(getenv "make_${1}_san_${test}")
        checkstatus failed "Failure: ${1}-bit santize with ${test} install failed." $(getenv "install_${1}_san_${test}")
        checkstatus failed "Failure: ${1}-bit santize with ${test} tests failed."   $(getenv "tests_${1}_san_${test}")
    done;
}

checktests() {
    if [ -z "$NOMAKE" ]; then
        echo "TEST RESULTS:"
        for arch in ${archlist};do
            checktestarch $arch
        done
        checkstatus abort "Failure: One or more tests have failed (see above)." $failed
        GREEN $COLOR
        echo "SUCCESS"
        NORMAL $COLOR
    fi
}

sanitize() {
    ### Build with sanitizers and run tests with each sanitizer
    if [ ! -z "$SANITIZE" ]
    then
        for test in ${SANITIZE}; do
            echo Doing sanitize ${test}
            MDSPLUS_DIR=/workspace/tests/${1}-san-${test}/buildroot;
            config_test $@ --enable-sanitize=${test} --disable-java
            if [ "$status" = "111" ]; then
                echo "Sanitizer ${test} not supported. Skipping."
            elif [ "$status" = "0" ]; then
              if [ -z "$NOMAKE" ]; then
                $MAKE
                checkstatus abort "Failure compiling $1-bit with sanitize-${test}." $?
                $MAKE install
                checkstatus abort "Failure installing $1-bit with sanitize-${test}." $?
                :&& tio 1800 $MAKE -k tests 2>&1
                checkstatus tests_${1}_san_${test} "Failure testing $1-bit with sanitize-${test}." $?
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
    MDSPLUS_DIR=/workspace/tests/$1/buildroot;
    config_test $@
   if [ -z "$NOMAKE" ]; then
    $MAKE
    checkstatus abort "Failure compiling $1-bit." $?
    $MAKE install
    checkstatus abort "Failure installing $1-bit." $?
    ### Run standard tests
    :&& tio 600 $MAKE -k tests 2>&1
    checkstatus tests_$1 "Failure testing $1-bit." $?
    if [ ! -z "$VALGRIND_TOOLS" ]
    then
        ### Test with valgrind
        to=$( gettimeout $VALGRIND_TOOLS )
        :&& tio $to  $MAKE -k tests-valgrind 2>&1
        checkstatus tests_${1}_val "Failure testing $1-bit with valgrind." $?
    fi
   fi
    popd
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
    case "$BRANCH" in
     stable) export BNAME="";;
      alpha) export BNAME="-alpha";;
          *) export BNAME="-other";;
    esac
    if [ "$RELEASE" = "yes" ]
    then
        set +e
        buildrelease
    fi
    if [ -z "$NOMAKE" ] && [ "$PUBLISH" = "yes" ]
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
