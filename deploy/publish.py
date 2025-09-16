#!/usr/bin/env python3

import os
import json
import shutil
import argparse
import platform
import subprocess
import sys

# Get the path to deploy/
deploy_dir = os.path.dirname(os.path.abspath(__file__))

# Get the path to the root of the repository
source_dir = os.path.dirname(deploy_dir)

parser = argparse.ArgumentParser()

parser.add_argument(
    '--distdir',
    help='The directory to publish repo information into.',
    required=True,
)

parser.add_argument(
    '--certdir',
    help='The directory containing certificates for signing packages and repo metadata.',
    required=True,
)

parser.add_argument(
    '--publish-info',
    help='Path to the mdsplus-publish.json file containing the information for this release.',
    required=True,
)

args = parser.parse_args()

file = open(args.publish_info, 'rt')
publish_info = json.load(file)

# Don't error if publish_info['dockerimage'] is None
docker = shutil.which('docker')
if docker is None:
    print('Unable to find `docker`')
    exit(1)

# TODO:
staging_dist_dir = os.path.join(os.getcwd(), 'dist', publish_info['distname'])
publish_dist_dir = os.path.join(args.distdir, publish_info['distname'])

release_version_filename = os.path.join(publish_dist_dir, f"{publish_info['flavor']}_{publish_info['arch']}_version")
if os.path.exists(release_version_filename):
    previous_version = open(release_version_filename, 'rt').read().strip()

    previous_version_tuple = tuple(previous_version.split('.'))
    current_version_tuple = tuple(publish_info['version'].split('.'))

    # Tuple's can be used to compare versions
    # e.g. (1, 2, 3) < (1, 3, 0)
    if previous_version_tuple >= current_version_tuple:
        print(f'Published version is already {previous_version}, skipping')
        exit(0)

os.makedirs(publish_dist_dir, exist_ok=True)

# TODO: Detect python3 instead of assuming it?
platform_publish_script = os.path.join(deploy_dir, f"platform/{publish_info['platform']}/{publish_info['platform']}_publish.py")
command = f"python3 {platform_publish_script} --flavor {publish_info['flavor']} --arch {publish_info['arch']} --version {publish_info['version']}"

if publish_info['dockerimage'] is None:

    result = subprocess.run(
        [ '/bin/bash', '-c', command ]
    )

    if result.returncode != 0:
        print(f'Failed to run `{command}`')
        exit(1)

else:

    docker_args = [
        '--rm',
        f'--volume={source_dir}:{source_dir}',
        f'--volume={staging_dist_dir}:/release',
        f'--volume={publish_dist_dir}:/publish',
        f'--volume={args.certdir}:/sign_keys:ro',
        f'--workdir={os.getcwd()}', # ?
    ]

    if platform.system() != 'Windows':
        docker_args.append(f'--user={os.getuid()}:{os.getgid()}')

    docker_args.append(publish_info['dockerimage'])

    docker_entrypoint = [ '/bin/bash', '-c', command ]

    result = subprocess.run(
        [
            docker, 'run',
        ] + docker_args + docker_entrypoint,
    )

    if result.returncode != 0:
        print(f'Failed to run `{docker} run {" ".join(docker_args)} {docker_entrypoint}` ')
        exit(1)

print(f"Updating {release_version_filename} to {publish_info['version']}")
with open(release_version_filename, 'wt') as release_version_file:
    release_version_file.write(publish_info['version'] + '\n')
