#!/bin/bash
#
# platform/windows/windows_build.sh
#
# Invoked by mdsplus/deploy/platform/platform_build.sh for windows platform.
#
# Run docker image to build mdsplus
#
set -e
if [ "${RELEASE}" = "yes" -o "${PUBLISH}" = "yes" ]
then
    RELEASEDIR=${RELEASEDIR}
    mkdir -p ${RELEASEDIR}/${BRANCH}
else
    RELEASEDIR=""
fi
if [ "${PUBLISH}" = "yes" ]
then
    PUBLISHDIR=${PUBLISHDIR}
    mkdir -p ${PUBLISHDIR}/${BRANCH}
fi
set +e
rundocker
