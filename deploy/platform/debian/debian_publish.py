
import os
import glob
import shutil
import argparse
import subprocess

parser = argparse.ArgumentParser()

parser.add_argument(
    '--flavor',
    help='The release flavor to publish, either alpha or stable.',
    required=True,
)

parser.add_argument(
    '--arch',
    help='The architecture to publish.',
    required=True,
)

parser.add_argument(
    '--version',
    help='The release version to publish.',
    required=True,
)

args = parser.parse_args()

# This needs to contain all of the architectures that this platform builds for
all_arches = [ 'amd64', 'arm64' ]

rsync = shutil.which('rsync')
if rsync is None:
    print('Unable to find `rsync`')
    exit(1)

reprepro = shutil.which('reprepro')
if reprepro is None:
    print('Unable to find `reprepro`')
    exit(1)

# The /sign_keys directory is mounted read-only from docker, but GPG needs to have read-write access to it
# for some stupid reason, so we copy .gnupg to /tmp/
result = subprocess.run([rsync, '-a', '/sign_keys/.gnupg', '/tmp'])
sign_env = os.environ.copy()
sign_env['HOME'] = '/tmp'

release_component_dir = os.path.join('/release', args.flavor)
publish_component_dir = os.path.join('/publish', args.flavor)

print('Building repo')

os.makedirs(os.path.join('/release/repo/conf'), exist_ok=True)
os.makedirs(os.path.join('/release/repo/db'), exist_ok=True)
os.makedirs(os.path.join('/release/repo/dists'), exist_ok=True)
os.makedirs(os.path.join('/release/repo/pool'), exist_ok=True)

distributions_filename = '/release/repo/conf/distributions'
if not os.path.exists(distributions_filename):
    distributions_lines = [
        'Origin: MDSplus Development Team',
        'Label: MDSplus',
        'Codename: MDSplus',
        f"Architectures: {' '.join(all_arches)}",
        'Components: alpha stable cmake', # TODO: Remove cmake
        'Description: MDSplus packages',
        'SignWith: MDSplus',
        '',
        'Origin: MDSplus Development Team', # TODO: Remove MDSplus-previous
        'Label: MDSplus-previous',
        'Codename: MDSplus-previous',
        f"Architectures: {' '.join(all_arches)}",
        'Components: alpha stable cmake', # TODO: Remove cmake
        'Description: Previous MDSplus packages',
        'SignWith: MDSplus',
        '',
    ]

    with open(distributions_filename, 'wt') as file:
        file.write('\n'.join(distributions_lines))

    subprocess.run(
        ['/usr/bin/reprepro', 'clearvanished'],
        cwd='/release/repo',
        env=sign_env,
    )

# TODO: Take from mdsplus-publish.json ?
release_deb_filenames = glob.glob(os.path.join(release_component_dir, f'DEBS/{args.arch}/*{args.version}_*.deb'))
for deb in release_deb_filenames:
    print('Including', deb)
    result = subprocess.run(
        [reprepro, '-V', '-C', args.flavor, 'includedeb', 'MDSplus', deb ],
        cwd='/release/repo',
        env=sign_env
    )
    if result.returncode != 0:
        print(f'Failure: Problem installing {deb} into repository.')
        exit(1)

# TODO: Do we still need to do this to both /release and /publish?

publish_deb_dir = os.path.join(publish_component_dir, f'DEBS/{args.arch}/')
os.makedirs(publish_deb_dir, exist_ok=True)
for deb in release_deb_filenames:
    shutil.copy2(deb, publish_deb_dir)

if not os.path.isdir('/publish/repo'):
    subprocess.run([rsync, '-a', '/release/repo', '/publish/'])

shutil.copy2(os.path.join('/release/repo', 'conf/distributions'), os.path.join('/publish/repo', 'conf/'))
result = subprocess.run(
    [reprepro, 'clearvanished'],
    cwd='/publish/repo'
)

publish_deb_filenames = glob.glob(os.path.join(publish_component_dir, f'DEBS/{args.arch}/*{args.version}_*.deb'))
result = subprocess.run(
    [reprepro, '-V', '--keepunused', '-C', args.flavor, 'includedeb', 'MDSplus', *publish_deb_filenames ],
    cwd='/publish/repo',
    env=sign_env,
)
if result.returncode != 0:
    # This will completely regenerate the index files, it should only be used to repair a broken dist directory
    # or to generate a new one
    result = subprocess.run(
        reprepro, 'export', 'MDSplus',
        cwd='/publish/repo',
        env=sign_env,
    )
    if result.returncode != 0:
        print('Failure: Problem installing {args.flavor} into debian repository.')
        exit(1)

# TODO: MDSplus-previous ?
# last_release_info_filename = '/publish/{flavor}_{os_name}'