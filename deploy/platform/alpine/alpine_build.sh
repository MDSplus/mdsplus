#!/bin/bash
#
# platform/alpine/alpine_build.sh
#
# Invoked by mdsplus/deploy/platform/platform_build.sh for alpine platform.
#
# Run docker image to build mdsplus
#
# Enable arm emulation support in host kernel
#docker run --rm --privileged multiarch/qemu-user-static:register --reset
#
default_build
rundocker
