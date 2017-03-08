#!/bin/bash
#
# platform/debian/build.sh
#
# Invoked by mdsplus/deploy/platform/platform_build.sh for windows platform.
#
# Run docker image to build mdsplus
#
default_build
if [ "${RELEASE}" = "yes" ]
then
  # clean up repobefor creating a new release
  rm -Rf ${RELEASEDIR}/${BRANCH}/DEBS &>/dev/null
  rm -Rf ${RELEASEDIR}/repo           &>/dev/null
fi
rundocker
