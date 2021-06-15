# Sourced by mdsplus/deploy/platform/platform_build.sh
default_build
if [ "${RELEASE}" = "yes" ]
then
  # clean up repo before creating a new release
  rm -Rf ${RELEASEDIR}/repo
fi
rundocker
