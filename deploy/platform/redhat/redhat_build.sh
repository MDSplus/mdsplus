#!/bin/bash
#
# platform/redhat/redhat_build.sh
#
# Invoked by mdsplus/deploy/platform/platform_build.sh for windows platform.
#
# Run docker image to build mdsplus
#
set -e
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
rundocker
