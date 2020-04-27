#!/bin/bash
#
# Update expected packaging files
#
# Run in a separate build directory outside of source directory for best results.
#
# For example:
#
#     mkdir ~/build
#     cd ~/build
#     ~/mdsplus/deploy/updatepkg.sh
#
# This will update the expected packaging content files in
# ~/mdsplus/deploy/packaging... If the changes, if any, look good commit
# these changes to your branch before making a pull request.
#
# Note this requires a running docker service on your system and your
# account to be enabled to run docker.
#
# ensure we are in source folder, so build path is found
if [ -z $1 ]
then
logfile=/dev/stdout
logmessage=
else
logfile=$(realpath $1)
logmessage=" Check '$logfile' for details."
fi
cd $(dirname $0)/..


echo "$(date) updating package files for $(pwd)"
# bootstrap makefiles
deploy/build.sh --os=bootstrap > "$logfile" 2>&1
# build jars on first cycle (build on debian9-64)
jars=""
build_dir="$(pwd)/build/updatepkg"
# debian-amd64 debian-i386 debian-armhf redhat-*
for os in debian9-64 debian7-32 raspberrypi fc25
do
    echo "$(date) Build of $os starting"
    mkdir -p ${build_dir}/$os
    if ( deploy/build.sh --os=$os --workspace=${build_dir}/$os --test=skip --release ${jars} --updatepkg >> "$logfile" 2>&1 )
    then
	echo "$(date) Build of $os completed successfully"
    else
	stat=$?
	echo "$(date) Build of $os failed.${logmessage}"
	exit $stat
    fi
    # use jars from first build in subsequention cycles
    jars="--jars-dir=${build_dir}/debian9-64/releasebld/64"
done
