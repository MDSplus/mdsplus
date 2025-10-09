
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

parser.add_argument(
    '--release-dir',
    help='The directory containing packages from the build.',
    required=True,
)

parser.add_argument(
    '--publish-dir',
    help='The directory to publish packages and repository information into.',
    required=True,
)

parser.add_argument(
    '--cert-dir',
    help='The directory containing certificates for signing.',
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

# The args.cert_dir directory is mounted read-only from docker, but GPG needs to have read-write access to it
# for some stupid reason, so we copy .gnupg to /tmp/
result = subprocess.run([rsync, '-a', os.path.join(args.cert_dir, '.gnupg'), '/tmp'])
sign_env = os.environ.copy()
sign_env['HOME'] = '/tmp'

release_component_dir = os.path.join(args.release_dir, args.flavor)
publish_component_dir = os.path.join(args.publish_dir, args.flavor)

release_repo_dir = os.path.join(args.release_dir, 'repo')
publish_repo_dir = os.path.join(args.publish_dir, 'repo')

print('Building repo')

os.makedirs(os.path.join(release_repo_dir, 'conf'), exist_ok=True)
os.makedirs(os.path.join(release_repo_dir, 'db'), exist_ok=True)
os.makedirs(os.path.join(release_repo_dir, 'dists'), exist_ok=True)
os.makedirs(os.path.join(release_repo_dir, 'pool'), exist_ok=True)

distributions_filename = os.path.join(release_repo_dir, 'conf/distributions')
if not os.path.exists(distributions_filename):
    distributions_lines = [
        'Origin: MDSplus Development Team',
        'Label: MDSplus',
        'Codename: MDSplus',
        f"Architectures: {' '.join(all_arches)}",
        'Components: alpha stable',
        'Description: MDSplus packages',
        'SignWith: MDSplus',
        '',
        'Origin: MDSplus Development Team', # TODO: Remove MDSplus-previous
        'Label: MDSplus-previous',
        'Codename: MDSplus-previous',
        f"Architectures: {' '.join(all_arches)}",
        'Components: alpha stable',
        'Description: Previous MDSplus packages',
        'SignWith: MDSplus',
        '',
    ]

    with open(distributions_filename, 'wt') as file:
        file.write('\n'.join(distributions_lines))

    subprocess.run(
        [reprepro, 'clearvanished'],
        cwd=release_repo_dir,
        env=sign_env,
    )

# TODO: Take from mdsplus-publish.json ?
release_deb_filenames = glob.glob(os.path.join(release_component_dir, f'DEBS/{args.arch}/*{args.version}_*.deb'))
for deb in release_deb_filenames:
    print('Including', deb)
    result = subprocess.run(
        [reprepro, '-V', '-C', args.flavor, 'includedeb', 'MDSplus', deb ],
        cwd=release_repo_dir,
        env=sign_env
    )
    if result.returncode != 0:
        print(f'Failure: Problem installing {deb} into repository.')
        exit(1)

# TODO: Do we still need to do this to both args.release_dir and args.publish_dir?

publish_deb_dir = os.path.join(publish_component_dir, f'DEBS/{args.arch}/')
os.makedirs(publish_deb_dir, exist_ok=True)
for deb in release_deb_filenames:
    shutil.copy2(deb, publish_deb_dir)

if not os.path.isdir(publish_repo_dir):
    subprocess.run([rsync, '-a', release_repo_dir, args.publish_dir])

shutil.copy2(os.path.join(release_repo_dir, 'conf/distributions'), os.path.join(publish_repo_dir, 'conf/'))
result = subprocess.run(
    [reprepro, 'clearvanished'],
    cwd=publish_repo_dir
)

publish_deb_filenames = glob.glob(os.path.join(publish_component_dir, f'DEBS/{args.arch}/*{args.version}_*.deb'))
result = subprocess.run(
    [reprepro, '-V', '--keepunused', '-C', args.flavor, 'includedeb', 'MDSplus', *publish_deb_filenames ],
    cwd=publish_repo_dir,
    env=sign_env,
)
if result.returncode != 0:
    # This will completely regenerate the index files, it should only be used to repair a broken dist directory
    # or to generate a new one
    result = subprocess.run(
        reprepro, 'export', 'MDSplus',
        cwd=publish_repo_dir,
        env=sign_env,
    )
    if result.returncode != 0:
        print('Failure: Problem installing {args.flavor} into debian repository.')
        exit(1)

# TODO: MDSplus-previous ?
# last_release_info_filename = '{args.publish_dir}/{flavor}_{os_name}'