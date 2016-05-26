#!/bin/bash
#
# platform/redhat/build.sh
#
# Invoked by mdsplus/deploy/build.sh for redhat platforms.
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

if [ "${RELEASE}" = "yes" -o "${PUBLISH}" = "yes" ]
then
    RELEASEDIR=${RELEASEDIR}/${BRANCH}
    mkdir -p ${RELEASEDIR}
else
    RELEASEDIR=""
fi
if [ "${PUBLISH}" = "yes" ]
then
    PUBLISHDIR=${PUBLISHDIR}/${BRANCH}
    mkdir -p ${PUBLISHDIR}
fi
set +e
docker run -a stdout -a stderr --cidfile=${WORKSPACE}/${OS}_docker-cid \
       -u $(id -u):$(id -g) \
       -e "BRANCH=$BRANCH" \
       -e "DISTNAME=$DISTNAME" \
       -e "OS=$OS" \
       -e "RELEASE_VERSION=$RELEASE_VERSION"  \
       -e "TEST=$TEST" \
       -e "TEST_FORMAT=$TEST_FORMAT" \
       -e "mdsevent_port=$EVENT_PORT" \
       -e "RELEASE=$RELEASE" \
       -e "PUBLISH=$PUBLISH" \
       -e "SANITIZE=$SANITIZE" \
       -e "VALGRIND_TOOLS=$VALGRIND_TOOLS" \
       -e "UPDATEPKG=$UPDATEPKG" \
       -e "PLATFORM=$PLATFORM" \
       -e "COLOR=$COLOR" \
       -v $(realpath ${SRCDIR}):/source \
       -v ${WORKSPACE}:/workspace \
       $(volume "${RELEASEDIR}" /release) \
       $(volume "${PUBLISHDIR}" /publish) \
       $(volume "$KEYS" /sign_keys) \
       ${DOCKERIMAGE} /source/deploy/platform/$PLATFORM/redhat_docker_build.sh
status=$?
if [ -r ${WORKSPACE}/${OS}_docker-cid ]
then
    sleep 3
    docker rm $(cat ${WORKSPACE}/${OS}_docker-cid)
    rm -f ${WORKSPACE}/${OS}_docker-cid
fi
exit $status
