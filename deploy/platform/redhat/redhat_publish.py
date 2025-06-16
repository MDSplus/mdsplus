
import os
import glob
import shutil
import tempfile
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

# os_name = '' # This seems hacky, TODO replace with _{arch} or _version instead

# This needs to contain all of the architectures that this platform builds for
all_arches = [ 'x86_64' ] # TODO: 32-bit? ARM?

rsync = shutil.which('rsync')
if rsync is None:
    print('Unable to find `rsync`')
    exit(1)

createrepo = shutil.which('createrepo')
if createrepo is None:
    print('Unable to find `createrepo`')
    exit(1)

rpmsign = shutil.which('rpmsign')
if rpmsign is None:
    print('Unable to find `rpmsign`')
    exit(1)

gpg = shutil.which('gpg')
if gpg is None:
    print('Unable to find `gpg`')
    exit(1)

result = subprocess.run(
    [ createrepo, '-h' ],
    stdout=subprocess.PIPE,
    stderr=subprocess.STDOUT,
)

deltas_args = []
if '--deltas' in result.stdout.decode():
    deltas_args = [ '--deltas' ]

# The /sign_keys directory is mounted read-only from docker, but GPG needs to have read-write access to it
# for some stupid reason, so we copy .gnupg to /tmp/
result = subprocess.run([rsync, '-a', '/sign_keys/.gnupg', '/tmp'])
sign_env = os.environ.copy()
sign_env['HOME'] = '/tmp'
sign_env['GNUPGHOME'] = '/tmp/.gnupg'

release_component_dir = os.path.join('/release', args.flavor)
publish_component_dir = os.path.join('/publish', args.flavor)

os.makedirs(publish_component_dir, exist_ok=True)

for filename in os.listdir(release_component_dir):
    if filename == 'repodata':
        continue

    subprocess.run([rsync, '-a', os.path.join(release_component_dir, filename), publish_component_dir])

print('Signing packages')

redhat_package_version = '-'.join(args.version.rsplit('.', maxsplit=1)) # 1.2-3
rpm_filenames = glob.glob(f'/publish/{args.flavor}/RPMS/*/*{redhat_package_version}*.rpm')

result = subprocess.run(
    [ rpmsign, '--define', '_gpg_name MDSplus', '--addsign', *rpm_filenames ],
    env=sign_env
)

if result.returncode != 0:
    print('Failure: Problem signing rpm files.')
    exit(1)

print('Building repo')

tempdir = tempfile.mkdtemp()

update_args = []
repodata_dir = os.path.join(publish_component_dir, 'RPMS/repodata')
if os.path.isdir(repodata_dir):
    subprocess.run([rsync, '-a', repodata_dir, tempdir])
    update_args = ['--update', '--cachedir', f'{publish_component_dir}/cache', *deltas_args ]

result = subprocess.run(
    [ createrepo, '-q', *update_args, '-o', tempdir, os.path.join(publish_component_dir, 'RPMS') ],
)

if result.returncode != 0:
    print('Failure: Problem creating rpm repository in /publish!')
    exit(1)

repomd_asc_filename = os.path.join(tempdir, 'repodata/repomd.xml.asc')
if os.path.exists(repomd_asc_filename):
    result = os.remove(repomd_asc_filename)
    # TODO: Error handling?

result = subprocess.run(
    [ gpg, '--local-user', 'MDSplus', '--detach-sign', '--armor', os.path.join(tempdir, 'repodata/repomd.xml') ],
    env=sign_env
)
# TODO: Error handling?

subprocess.run([rsync, '-a', os.path.join(tempdir, 'repodata'), f'{publish_component_dir}/RPMS/'])
