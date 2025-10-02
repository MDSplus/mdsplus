
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
package_filename = glob.glob(os.path.join(args.release_dir, f'mdsplus_{args.flavor}_{args.version}-*-{args.arch}.tgz'))

shutil.copy2(package_filename, args.publish_dir)
