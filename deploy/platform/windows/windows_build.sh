#!/bin/bash
#
# platform/windows/windows_build.sh
#
# Invoked by mdsplus/deploy/build.sh for windows platform.
#
# Run docker image to build mdsplus
#
printenv
set -e
volume() {
    if [ ! -z "$1" ]
    then
	echo "-v $(realpath ${1}):${2}"
    fi
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

if [ "${RELEASE}" = "yes" ]
then
    mkdir -p ${RELEASEDIR}/${BRANCH}
    rm -Rf ${RELEASEDIR}/${BRANCH}/*
    releasedir=${RELEASEDIR}
fi

if [ "${PUBLISH}" = "yes" ]
then
    mkdir -p ${PUBLISHDIR}
    publishdir=${PUBLISHDIR}
    releasedir=${RELEASEDIR}
fi
set +e

spacedelim() {
    if [ ! -z "$1" ]
    then
	IFS=',' read -ra ARR <<< "$1"
	ans="${ARR[*]}"
    fi
    echo $ans
}

arches=($(spacedelim ${ARCH}))
idx=0
docker run -t -a stdout -a stderr --cidfile=${WORKSPACE}/${OS}_docker-cid \
       -u $(id -u):$(id -g) \
       -e "BRANCH=$BRANCH" \
       -e "DISTNAME=$DISTNAME" \
       -e "OS=$OS" \
       -e "RELEASE_VERSION=$RELEASE_VERSION"  \
       -e "TEST=$TEST" \
       -e "TESTFORMAT=$TEST_FORMAT" \
       -e "mdsevent_port=$EVENT_PORT" \
       -e "RELEASE=$RELEASE" \
       -e "PUBLISH=$PUBLISH" \
       -e "PLATFORM=$PLATFORM" \
       -e "COLOR=${COLOR}" \
       -e "GIT_COMMIT=${GIT_COMMIT}" \
       -e "WINREMBLD=${WINREMBLD}" \
       -e "WINHOST=${WINHOST}" \
       -v $(realpath ${SRCDIR}):/source \
       -v ${WORKSPACE}:/workspace \
       $(volume "${WINBLD}" /winbld) \
       $(volume "${releasedir}" /release) \
       $(volume "${publishdir}" /publish) \
       $(volume "$KEYS" /sign_keys) \
       ${DOCKERIMAGE} /source/deploy/platform/${PLATFORM}/${PLATFORM}_docker_build.sh
status=$?
if [ -r ${WORKSPACE}/${OS}_docker-cid ]
then
    sleep 3
    docker rm $(cat ${WORKSPACE}/${OS}_docker-cid)
    rm -f ${WORKSPACE}/${OS}_docker-cid
fi
if [ ! "$status" = "0" ]
then
    RED $COLOR
    cat <<EOF >&2
======================================================

Docker ${PLATFORM}_docker_build.sh returned failure   
status when exiting from ${DOCKERIMAGE}

======================================================
EOF
    NORMAL $COLOR
    exit 1
fi
