# MDSplus Windows Builder
#
# Build image using:
#
#    docker build -t mdsplus:buildWindows .
#
# Run this using docker with the command:
#
#    docker run --rm=true -e BUILD_NUMBER=${BUILD_NUMBER} -e JOB_NAME="${JOB_NAME}" -v /mdsplus:/mdsplus mdsplus:buildWindows
#
# Where: /mdsplus on the host is a mount of alchome:/scratch/mdsplus
#        BUILD_NUMBER and JOB_NAME defined by hudson with the job is run. That is used to add a reference to the job alongside the kit
#        on the download page.
#        BUILD_FLAVOR (i.e. alpha | beta | stable | all)
#
#
FROM fedora

MAINTAINER Tom Fredian, version: 0.1

RUN yum -y install autoconf automake cvs 'mingw*-gcc-*' 'mingw*-readline' 'mingw*-dlfcn*' \
    	   	   'mingw*-readline*' 'mingw32-nsis*' '*-openjdk-devel*' wget binutils make \
		   mono-devel
RUN yum -y update
RUN mkdir /mdsplus
WORKDIR /tmp
RUN cvs -d :pserver:MDSguest:MDSguest@www.mdsplus.org:/mdsplus/repos co deploy;
WORKDIR /tmp/deploy
ENTRYPOINT cvs -q update -d; \
	   export DIST=windows; \
	   export LABVIEW_DIR=/mdsplus/extras/Labview-x86; \
	   export LABVIEW64_DIR=/mdsplus/extras/Labview-x86_64; \
	   export IDL_DIR=/mdsplus/extras/idl-x86; \
	   export IDL64_DIR=/mdsplus/extras/idl-x86_64; \ 
	   export JDK_DIR=/etc/alternatives/java_sdk; \
	   export WORKSPACE=/tmp/deploy; \
	   export MDSPLUS_DIST=/mdsplus/dist; \
           export MDSPLUS_CERTS=/mdsplus/certs; \
	   python release.py deploy ${BUILD_FLAVOR}
