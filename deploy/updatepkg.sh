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
jars=""
for os in fc22 ubuntu14 raspberrypi
do
    echo "$(date) Build of $os starting"
    if ( $(dirname $0)/build.sh --os=$os --release ${jars} --updatepkg > ./updatepkg.log 2>&1 )
    then
	echo "$(date) Build of $os completed successfully"
    else
	stat=$?
	echo "$(date) Build of $os failed. See ./updatepkg.log for details"
	exit $stat
    fi
    jars="--jars-dir=build/fc22/releasebld/64"
done

