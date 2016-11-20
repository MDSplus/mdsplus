#!/bin/bash
#
# platform/debian/build.sh
#
# Invoked by mdsplus/deploy/platform/platform_build.sh for windows platform.
#
# Run docker image to build mdsplus
#
set -e
if [ "${RELEASE}" = "yes" ]
then
    mkdir -p ${RELEASEDIR}/${BRANCH}
    rm -Rf ${RELEASEDIR}/${BRANCH}/DEBS
    rm -Rf ${RELEASEDIR}/repo
    releasedir=${RELEASEDIR}
fi

if [ "${PUBLISH}" = "yes" ]
then
    mkdir -p ${PUBLISHDIR}
    publishdir=${PUBLISHDIR}
    releasedir=${RELEASEDIR}
fi
set +e
rundocker
