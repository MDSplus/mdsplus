#!/bin/bash
#
# runs $srcdir/bootstrap in a controlled maner
mkdir -p ${WORKSPACE}
cleanup() {
  docker rm $(cat ${WORKSPACE}/docker-cid)
  rm -f ${WORKSPACE}/docker-cid
}
trap cleanup EXIT
volume() {
  if [ ! -z "$1" ]
  then echo "-v $(realpath ${1}):${2}"
  fi
}
MVN="mvn -Dmaven.repo.local=/workspace/m2-repo -s /sign_keys/.m2/settings.xml -Dsettings.security=/sign_keys/.m2/settings-security.xml -DoutputDirectory=/workspace/target -DsourceDirectory=/mdsplus-api/src"
docker run -t -a stdout -a stderr --cidfile=${WORKSPACE}/docker-cid \
   -e MVN \
   -e "HOME=/workspace" \
   -v ${SRCDIR}/java/mdsplus-api:/mdsplus-api \
   -v ${WORKSPACE}:/workspace \
   $(volume "${KEYS}" /sign_keys) \
   ${DOCKERIMAGE} /bin/sh -c "cd /workspace;cp /mdsplus-api/pom.xml .;${MVN} versions:set -DnewVersion=${RELEASE_VERSION}&&${MVN} -DskipTests deploy"
