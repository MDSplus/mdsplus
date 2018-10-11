#!/bin/bash
#
# runs $srcdir/bootstrap in a controlled maner
mkdir -p ${WORKSPACE}
cleanup() {
  docker rm $(cat ${WORKSPACE}/docker-cid)
  rm -f ${WORKSPACE}/docker-cid
}
trap cleanup EXIT
docker run -t -a stdout -a stderr --cidfile=${WORKSPACE}docker-cid \
   -u $(id -u):$(id -g) --privileged \
   -e "HOME=/workspace" \
   -v ${SRCDIR}:${DOCKER_SRCDIR} \
   -v ${WORKSPACE}:/workspace \
   ${DOCKERIMAGE} /bin/sh -c "cd ${SRCDIR};./bootstrap"
