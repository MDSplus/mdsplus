#!/bin/bash
#
# runs $srcdir/bootstrap in a controlled manner
cid=$(mktemp -d)/bootstrap-docker-cid
cleanup() {
 if [ -f $cid ]
 then
  docker rm -f $(cat $cid)
  rm -f $cid
  rmdir $(dirname $cid)
 fi
}
trap cleanup EXIT INT
docker run -t -a stdout -a stderr --cidfile=$cid \
   -u $(id -u):$(id -g) --privileged \
   -e "HOME=/tmp" \
   -v "${SRCDIR}:${DOCKER_SRCDIR}" \
   ${DOCKERIMAGE} "${DOCKER_SRCDIR}/bootstrap"
