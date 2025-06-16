
import os
import argparse
import shutil
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

osslsigncode = shutil.which('osslsigncode')
if osslsigncode is None:
    print('Unable to find `osslsigncode`')
    exit(1)

release_component_dir = os.path.join('/release', args.flavor)
publish_component_dir = os.path.join('/publish', args.flavor)

os.makedirs(publish_component_dir, exist_ok=True)

windows_package_version = '-'.join(args.version.rsplit('.', maxsplit=1)) # 1.2-3

bname = ''
if args.flavor != 'stable':
    bname = f'-{args.flavor}'

release_installer_filename = os.path.join(release_component_dir, f'MDSplus{bname}-{windows_package_version}-{args.arch}.exe')
publish_installer_filename = os.path.join(publish_component_dir, f'MDSplus{bname}-{windows_package_version}-{args.arch}.exe')
# This is stored in /tmp/ because osslsigncode refuses to overwrite an existing file
signed_installer_filename = os.path.join('/tmp', f'MDSplus-{args.flavor}-{windows_package_version}-{args.arch}-signed.exe')

result = subprocess.run(
    [
        osslsigncode, 'sign',
        '-certs', '/sign_keys/mdsplus.spc', # What is an spc?
        '-key', '/sign_keys/mdsplus.pvk', # What is a pvk? Putty? Private Key?
        '-pass', 'mdsplus', # TODO: Don't.. do this
        '-n', 'MDSplus',
        '-i', 'http://www.mdsplus.org/',
        '-in', release_installer_filename,
        '-out', signed_installer_filename,
    ]
)

if result.returncode != 0:
    print('Failed to sign installer')
    exit(1)

shutil.copy2(signed_installer_filename, publish_installer_filename)
