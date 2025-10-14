
import os
import argparse
import shutil
import glob

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


# * may be `macports` or `homebrew`
package_filenames = glob.glob(os.path.join(args.release_dir, f'mdsplus_{args.flavor}_{args.version}_*_{args.arch}.tgz'))

flavor_dir = os.path.join(args.publish_dir, args.flavor)
os.makedirs(flavor_dir, exist_ok=True)

for filename in package_filenames:
    print(f'Copying {filename} to {flavor_dir}')
    shutil.copy2(filename, flavor_dir)
