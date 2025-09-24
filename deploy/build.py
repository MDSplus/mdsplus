#!/usr/bin/env python3

import sys

if not sys.version_info >= (3, 6):
    print('This script must be run with python >= 3.6')
    exit(1)

import argparse
import glob
import json
import math
import os
import platform
import shutil
import signal
import subprocess

from urllib import request
from datetime import datetime

# Get the path to deploy/
deploy_dir = os.path.dirname(os.path.abspath(__file__))

# Get the path to the root of the repository
source_dir = os.path.dirname(deploy_dir)

os_options = {}
for filename in glob.glob(os.path.join(deploy_dir, 'os/*.opts')):
    name = os.path.basename(filename).replace('.opts', '')

    if os.path.islink(filename):
        real_file = os.path.realpath(filename)
        real_name = os.path.basename(real_file).replace('.opts', '')

        if real_name not in os_options:
            os_options[real_name] = []
        
        os_options[real_name].append(name)
    
    else:
        if name not in os_options:
            os_options[name] = []

os_options_help_text = 'The following values (and aliases) for --os are available:\n'
for name, alias_list in sorted(os_options.items()):
    os_options_help_text += f'  {name}'
    if len(alias_list) > 0:
        os_options_help_text += f" : {', '.join(sorted(alias_list, reverse=True))}"
    os_options_help_text += '\n'

parser = argparse.ArgumentParser(
    # Use a custom formatter class to allow our --os options to display properly in --help
    # Use max_help_position to increase the width of the left column in --help
    formatter_class=lambda prog: argparse.RawDescriptionHelpFormatter(prog, max_help_position=45),
    epilog=os_options_help_text,
)

# Configuration

parser.add_argument(
    '--os',
    help='The OS definition to use (see below), which will reference `deploy/os/{--os}.opts` for additional parameters to this script and `deploy/os/{--os}.env` for additional environment variables. This will also change the default for --workspace to be `workspace-{--os}/`',
)

parser.add_argument(
    '--env-file',
    help='Path to a file containing additional environment variables to set in the form of NAME=VALUE. For docker builds, this will be evaluated inside the docker container.'
)

parser.add_argument(
    '--workspace',
    help='The directory that will contain the default build/install directories and helper scripts, defaults to `workspace/` or `workspace-{--os}/` if --os is specified. This will be relative to the source directory unless an absolute path is given.',
)

parser.add_argument(
    '-i', '--interactive',
    action='store_true',
    help='Drop into an interactive shell, allowing you to configure/build/install/test. Any other stage arguments will be ignored. This will attempt to clean your environment of references to $MDSPLUS_DIR if any are found.',
)

# default= is the value used if the argument is not specified
# const= is the value used if the argument is specified without a value
parser.add_argument(
    '-j', '--parallel',
    nargs='?',
    default=1,
    const=os.cpu_count(),
    metavar='THREADS',
    help='The number of parallel files to build or tests to run, defaults to `os.cpu_count()` if no value is specified.',
)

parser.add_argument(
    '--setup-vscode',
    action='store_true',
    help='Configure `.vscode/settings.json` for use with the CMake and clangd extensions, and generate `.vscode/launch.json` entries for each test.',
)

# Building

try:
    boolean_action = argparse.BooleanOptionalAction
except:
    # Hack for python < 3.9
    boolean_action = 'store_true'

parser.add_argument(
    '--configure',
    action=boolean_action,
    default=False,
    help='Configures CMake in `{--workspace}/build`, enabled automatically if CMakeCache.txt is not found.'
)

parser.add_argument(
    '--build',
    action=boolean_action,
    default=True,
    help='Builds the project in `{--workspace}/build`.'
)

# Hack for python < 3.9
if boolean_action == 'store_true':
    parser.add_argument(
        '--no-build',
        action='store_true',
        default=False,
    )

parser.add_argument(
    '--clean',
    action='store_true',
    default=False,
    help='Use with --build, will clean the project in `{--workspace}/build` before building.'
)

# Packaging

parser.add_argument(
    '--install',
    action=boolean_action,
    default=False,
    help='Install into `{--workspace}/install/usr/local/mdsplus`. Implied by --package. Sets CMAKE_INSTALL_PREFIX.',
)

parser.add_argument(
    '--package',
    action=boolean_action,
    default=False,
    help='Generates native package files and repository metadata in `{--workspace}/dist`. Generates tar files in `{--workspace}/package`, one containing the native package files and the other containing `{--workspace}/install/usr/local/mdsplus`'
)

parser.add_argument(
    '--verify-packages',
    action='store_true',
    default=False,
    help='When used with --package, it validates the contents of the generated packages against `deploy/packaging/{--platform}/*`.'
)

parser.add_argument(
    '--distname',
    help='Used by --package to determine the directory to generate repository information into, `{--workspace}/dist/{--distname}.',
)

parser.add_argument(
    '--platform',
    help='The platform type to build for. This controls how directories are named in the build folder, in preparation for packaging for a given platform type. Sets PLATFORM.',
)

parser.add_argument(
    '--arch',
    help='The architecture to label packages as. This should be used in conjunction with --toolchain when cross-compiling. Will attempt to autodetect from the current architecture.'
)

parser.add_argument(
    '--flavor',
    help='Used by --package to specify the release flavor, usually "alpha" or "stable". Will default to the prefix of the current git tag, if present. Required if --version is specified.'
)

parser.add_argument(
    '--version',
    help='Used by --package to specify the release version in the format "MAJOR.MINOR.PATCH". Will default to the suffix of the current git tag, if present. Required if --flavor is specified.'
)

# Testing

parser.add_argument(
    '--test',
    action=boolean_action,
    default=False,
    help='Run all tests and report the results. Use -j/--parallel to run tests in parallel. Use -R/--test-regex or --rerun-failed to control which tests are run.',
)

parser.add_argument(
    '--valgrind',
    nargs='?',
    const=True,
    metavar='TOOL',
    help='Specify a comma-separated list of valgrind tools to run for supported tests. An additional iteration of each test will be added for each tool. Leave blank to use all default tools. Cannot be used with --sanitize. Sets ENABLE_VALGRIND and VALGRIND_TOOLS.'
)

parser.add_argument(
    '--sanitize',
    metavar='FLAVOR',
    help='Configures the build to use the specified sanitizer flavor. Cannot be used with --valgrind. Sets ENABLE_SANITIZE.',
)

parser.add_argument(
    '--rerun-failed',
    action='store_true',
    default=False,
    help='Use with --test to run only the tests that previously failed.',
)

parser.add_argument(
    '-R', '--test-regex',
    help='Use with --test to run only the tests that match this regex.',
)

parser.add_argument(
    '--output-junit',
    action='store_true',
    default=False,
    help='Use with --test to store jUnit-style test results in `{--workspace}/mdsplus-junit.xml`.',
)

parser.add_argument(
    '--junit-suite-name',
    help='Use with --output-junit to set the name of the jUnit test suite. Defaults to --os if specified, or "mdsplus" if not.',
)

# Docker

parser.add_argument(
    '--dockerpull',
    action='store_true',
    help='Pull the latest docker image before creating the container.',
)

parser.add_argument(
    '--dockerimage',
    metavar='IMAGE',
    help='Create a docker container with this image, and run the build inside there. Can be combined with -i/--interactive to get a shell inside the docker container.',
)

parser.add_argument(
    '--dockernetwork',
    metavar='NETWORK',
    help='Create and use this docker network when creating the docker container.',
)

args, cmake_args = parser.parse_known_args()

if args.os is not None:

    opts_filename = os.path.join(deploy_dir, f'os/{args.os}.opts')
    env_filename = opts_filename.replace('.opts', '.env')

    if not os.path.exists(opts_filename):
        print(f'Unsupported --os={args.os}, ensure that deploy/os/{args.os}.opts exists.')
        exit(1)

    os_alias = None
    if os.path.islink(opts_filename):
        opts_filename = os.path.realpath(opts_filename)
        env_filename = opts_filename.replace('.opts', '.env')
        os_alias = os.path.basename(opts_filename).replace('.opts', '')
    
    opts = open(opts_filename).read().strip().split()

    # To allow command-line arguments to override those from .opts files, we need to parse them again after parsing the .opts ones
    args, cmake_args = parser.parse_known_args(args=opts + sys.argv[1:])

    if os.path.exists(env_filename):
        args.env_file = env_filename

    if os.path.exists(env_filename):
        args.env_file = env_filename

    if os_alias is not None:
        print()
        print(f'Using aliased --os={args.os} -> --os={os_alias}')
        
        args.os = os_alias

# Defaults

if args.workspace is None:
    if args.os is None:
        args.workspace = 'workspace'
    else:
        args.workspace = f'workspace-{args.os}'

if not os.path.isabs(args.workspace):
    args.workspace = os.path.join(source_dir, args.workspace)

if args.output_junit and args.junit_suite_name is None:
    if args.os is None:
        args.junit_suite_name = 'mdsplus'
    else:
        args.junit_suite_name = args.os

# Hack for python < 3.9
if boolean_action == 'store_true':
    if args.no_build:
        args.build = False

# Force --install if --package is specified
if args.package:
    args.install = True

# Only autodetect --platform on the actual system where packages will be built
if args.platform is None and args.dockerimage is None:
    if platform.system() == 'Windows':
        args.platform = 'windows'
    if platform.system() == 'Darwin':
        args.platform = 'macosx'
    elif os.path.exists('/etc/os-release'):
        id_list = []
        with open('/etc/os-release', 'rt') as filename:
            lines = filename.readlines()
            for line in lines:
                if '=' in line:
                    key, value = line.replace('"', '').split('=', maxsplit=1)
                    if key in [ 'ID', 'ID_LIKE' ]:
                        id_list.extend(value.split())

        if 'debian' in id_list:
            args.platform = 'debian'
        elif 'rhel' in id_list:
            args.platform = 'redhat'
        elif 'alpine' in id_list:
            args.platform = 'alpine'

# Directories

build_dir             = os.path.join(args.workspace, 'build')
install_dir           = os.path.join(args.workspace, 'install')
usr_local_mdsplus_dir = os.path.join(install_dir, 'usr/local/mdsplus')
testing_dir           = os.path.join(args.workspace, 'testing')
packages_dir          = os.path.join(args.workspace, 'packages')
dist_dir              = os.path.join(args.workspace, 'dist')

# System Configuration

# Environment variables must be handled before finding any programs
if args.env_file is not None and args.dockerimage is None:
    lines = open(args.env_file).readlines()
    for line in lines:
        name, value = line.split('=', maxsplit=1)
        
        # TODO: Improve
        result = subprocess.run(
            ['/bin/bash', '-c', f"echo {value}"],
            stdout=subprocess.PIPE,
        )
        value = result.stdout.decode().strip()

        os.environ[name] = value

cmake = shutil.which('cmake')
if cmake is None and args.dockerimage is not None:
    print('Unable to find `cmake`')
    exit(1)

ctest = shutil.which('ctest')
if ctest is None and args.dockerimage is not None:
    print('Unable to find `ctest`')
    exit(1)

# Causes readline to segfault when run through wine
if 'LC_CTYPE' in os.environ:
    del os.environ['LC_CTYPE']

# Utilities

def build_command_line():
    global args

    cli_args = []

    # You can't iterate over a argparse.Namespace, so we use vars()
    for name in vars(args):

        # Equivalent to calling args.{name}
        value = getattr(args, name)
        
        # argparse replaces '-' with '_', so we reverse this process
        name = name.replace('_', '-')

        if value is not None:
            if type(value) is bool:
                if value:
                    cli_args.append(f'--{name}')
                # elif name in ['configure', 'build', 'test', 'install', 'package']:
                elif name in ['build']:
                    cli_args.append(f'--no-{name}')
            else:
                cli_args.append(f'--{name}={value}')

    return cli_args

# CMake Configuration

cmake_cache_filename = os.path.join(build_dir, 'CMakeCache.txt')

def parse_cmake_cache():
    global build_dir, cmake_cache_filename

    cache = {}
    try:
        with open(cmake_cache_filename, 'rt') as f:
            for line in f.readlines():
                line = line.strip()
                if len(line) == 0 or line.startswith(('//', '#', )) or '-ADVANCED' in line:
                    continue
                try:
                    key, value = line.split('=', maxsplit=1)
                    name, type = key.split(':', maxsplit=1)
                    cache[name] = value
                except:
                    continue
    except FileNotFoundError:
        pass

    return cache

cmake_cache = parse_cmake_cache()

if len(cmake_cache) == 0:
    # If there is no CMake cache, we need to --configure
    print('No configure cache present, configuring')
    args.configure = True

elif 'MDSPLUS_CONFIGURE_COMPLETE' not in cmake_cache:
    # If we have not successfuly configured at least once, we need to --configure
    print('Previous configure attempt failed, reconfiguring')
    args.configure = True

# --fresh tells CMake to disregard the current cache and start over, so we need to do the same
if '--fresh' in cmake_args:
    cmake_cache = {}
    args.configure = True

def check_add_cmake_arg(arg):
    global args, cmake_args, cmake_cache

    if arg.startswith('-D') and '=' in arg:
        name, value = arg[2:].split('=', maxsplit=1)

        # Skip arguments that are already set in the cache
        if name in cmake_cache and cmake_cache[name] == value:
            return
    
    if arg not in cmake_args:
        args.configure = True
        cmake_args.append(arg)

# Attempt to filter out the arguments that are already set in the CMake cache
cmake_args_unfiltered = cmake_args.copy()
cmake_args = []
for arg in cmake_args_unfiltered:
    check_add_cmake_arg(arg)

check_add_cmake_arg(f'-DCMAKE_INSTALL_PREFIX={usr_local_mdsplus_dir}')

if args.platform is not None:
    check_add_cmake_arg(f'-DPLATFORM={args.platform}')

if args.flavor is None and args.version is not None:
    print('You must specify --flavor when using --version')
    exit(1)
elif args.version is None and args.flavor is not None:
    print('You must specify --version when using --flavor')
    exit(1)

# If --flavor and --version are specified, inform CMake
if args.flavor is not None and args.version is not None:
    release_tag = f'{args.flavor}_release-{"-".join(args.version.split("."))}'
    check_add_cmake_arg(f'-DRELEASE_TAG={release_tag}')
else:
    # If both are missing, clear the cached value from CMake
    if 'RELEASE_TAG' in cmake_cache:
        check_add_cmake_arg(f'-DRELEASE_TAG=')

if args.sanitize is not None and args.valgrind is not None:
    print()
    print('It is not recommended to run valgrind with a sanitizer')
    print()

if args.sanitize:
    check_add_cmake_arg(f'-DENABLE_SANITIZE={args.sanitize}')
else:
    check_add_cmake_arg(f'-DENABLE_SANITIZE=OFF')

if args.valgrind is not None:
    check_add_cmake_arg('-DENABLE_VALGRIND=ON')

    if type(args.valgrind) is str:
        check_add_cmake_arg(f'-DVALGRIND_TOOLS={args.valgrind}')
else:
    check_add_cmake_arg('-DENABLE_VALGRIND=OFF')

# Force --configure if no CMakeCache.txt is found or if new CMake options are specified
if not args.configure and len(cmake_args) != 0:
    args.configure = True

# Stages

def do_docker():

    docker = shutil.which('docker')
    if docker is None:
        print('Unable to find `docker`')
        exit(1)

    if args.dockerpull:
        print()
        print(f'Pulling docker image {args.dockerimage}')

        result = subprocess.run([ docker, 'pull', args.dockerimage ])
        if result.returncode != 0:
            print(f'Failed to pull docker image {args.dockerimage}')
            exit(1)
    
    os.makedirs(args.workspace, exist_ok=True)

    docker_args = [
        # Mount the workspace and source directory as absolute paths inside the docker
        f'--volume={args.workspace}:{args.workspace}',
        f'--volume={source_dir}:{source_dir}',
        
        # Working directory
        f'--workdir={args.workspace}',
        f'--env=HOME={args.workspace}',

        # HACK: To allow publish.py to know what docker image to run for publishing packages
        f'--env=DOCKERIMAGE={args.dockerimage}'
    ]

    # # Enable colors
    # if sys.stdout.isatty():
    #     docker_args.append('--tty')
    
    # TODO: Investigate
    # Without this, we lose track of the processes
    docker_args.append('--tty')

    if args.dockernetwork is not None:
        print(f'Creating docker network {args.dockernetwork}')
        result = subprocess.run([ docker, 'network', 'create', args.dockernetwork ])
        if result.returncode != 0:
            result = subprocess.run([ docker, 'network', 'inspect', args.dockernetwork ])
            if result.returncode != 0:
                print(f'Failed to create docker network {args.dockernetwork}')
                exit(1)
            print(f'Docker network {args.dockernetwork} already exists')

        docker_args.append(f'--network={args.dockernetwork}')

    if platform.system() != 'Windows':
        docker_args.append(f'--user={os.getuid()}:{os.getgid()}')

    docker_args.append(args.dockerimage)

    print()
    print('Docker arguments:')
    for arg in docker_args:
        print(f"    {arg}")

    passthrough_args = []
    for arg in build_command_line():

        # We don't want the .opts files to be parsed recursively
        if arg.startswith('--os='):
            continue

        # We don't want docker to run recursively
        if arg.startswith('--docker'):
            continue
        
        passthrough_args.append(arg)

    passthrough_args.extend(cmake_args)

    result = subprocess.run(
        [ docker, 'run', args.dockerimage, '/bin/sh', '-c', 'command -v python3' ],
        stdout=subprocess.PIPE,
    )
    if result.returncode != 0:
        print(f'Unable to find python3 in {args.dockerimage}')
        exit(1)

    docker_python3 = result.stdout.decode().strip()
    docker_entrypoint = [ docker_python3, os.path.abspath(__file__) ] + passthrough_args

    print('Docker entrypoint:')
    print(f"    {' '.join(docker_entrypoint)}")

    if args.interactive:

        subprocess.run(
            [
                docker, 'run',
                '--interactive',
                '--rm',
            ] + docker_args + docker_entrypoint,
        )

        if args.dockernetwork is not None:
            subprocess.run([ docker, 'network', 'rm', args.dockernetwork ])

    else:

        result = subprocess.run(
            [
                docker, 'run',
                '--detach',
            ] + docker_args + docker_entrypoint,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        if result.returncode != 0:
            print(f'Failed to run docker container: {result.stderr.decode()}')
            exit(1)
        
        container_id = result.stdout.decode().strip()

        docker_logs = subprocess.Popen(
            [
                docker, 'logs',
                '--follow',
                '--timestamps',
                container_id
            ],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        def kill_docker(signum, frame):
            print()
            print(f'Killing docker container {container_id}')
            
            subprocess.run([ docker, 'kill', container_id ])
            subprocess.run([ docker, 'rm', container_id ])

            if args.dockernetwork is not None:
                subprocess.run([ 'docker', 'network', 'rm', args.dockernetwork ])

            exit(0)

        signal.signal(signal.SIGINT, kill_docker)

        while True:
            line = docker_logs.stdout.readline()
            if not line:
                break

            print(line.decode().rstrip())

        signal.signal(signal.SIGINT, signal.SIG_DFL)

        # Perform an autopsy
        result = subprocess.run(
            [
                docker, 'inspect',
                container_id,
                '--format="{{.State.ExitCode}}"'
            ],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        exit_code = int(result.stdout.decode().strip().strip('"'))
        
        subprocess.run([ docker, 'rm', container_id ])

        if args.dockernetwork is not None:
            subprocess.run([ docker, 'network', 'rm', args.dockernetwork ])

        exit(exit_code)

def do_interactive():
    global args, cmake_args

    os.makedirs(args.workspace, exist_ok=True)

    do_configure_filename = os.path.join(args.workspace, 'do-configure.sh')
    with open(do_configure_filename, 'wt') as file:
        file.write('#!/bin/bash\n')
        file.write(f'cd "{build_dir}"\n')
        file.write(f"{cmake} {source_dir} -DCMAKE_INSTALL_PREFIX={usr_local_mdsplus_dir} {' '.join(cmake_args)} \"$@\"\n")
    os.chmod(do_configure_filename, 0o755)

    do_build_filename = os.path.join(args.workspace, 'do-build.sh')
    with open(do_build_filename, 'wt') as file:
        file.write('#!/bin/bash\n')
        file.write(f'cd "{build_dir}"\n')
        file.write(f'{cmake} --build "{build_dir}" "$@"\n')
    os.chmod(do_build_filename, 0o755)

    do_test_filename = os.path.join(args.workspace, 'do-test.sh')
    with open(do_test_filename, 'wt') as file:
        file.write('#!/bin/bash\n')
        file.write(f'cd "{source_dir}"\n')
        file.write(f'{sys.executable} "{__file__}" --workspace="{args.workspace}" --no-configure --no-build --test "$@"\n')
    os.chmod(do_test_filename, 0o755)

    do_install_filename = os.path.join(args.workspace, 'do-install.sh')
    with open(do_install_filename, 'wt') as file:
        file.write('#!/bin/bash\n')
        file.write(f'{cmake} --install "{build_dir}" "$@"\n')
    os.chmod(do_install_filename, 0o755)
    
    setup_filename = os.path.join(args.workspace, 'setup.sh')
    with open(setup_filename, 'wt') as file:
        file.write('\n')
        file.write('if [ -f /etc/mdsplus.conf ] || [ -f $HOME/.mdsplus ]; then\n')
        file.write('  echo "Unable to use setup.sh if /etc/mdsplus.conf or $HOME/.mdsplus exists"\n')
        file.write('else\n')
        file.write(f'  export PYTHONPATH=\"{usr_local_mdsplus_dir}/python\"\n')
        file.write(f'  export MDSPLUS_DIR=\"{usr_local_mdsplus_dir}\"\n')
        file.write('  source $MDSPLUS_DIR/setup.sh\n')
        file.write('fi\n')
    os.chmod(setup_filename, 0o755)

    # We require bash as it allows us control over $PS1 and --login --noprofile
    shell = '/bin/bash'

    if sys.stdout.isatty():
        reset = '\\e[0m'
        purple = '\\e[0;35m'
        green = '\\e[0;32m'
        turquoise = '\\e[0;36m'
    else:
        reset = ''
        purple = ''
        green = ''
        turquoise = ''

    # Start with a clean environment so we don't inherit anything pointing to the system MDSplus installation
    interactive_env = dict()

    passthrough_env_names = ['HOME', 'TERM', 'DISPLAY', 'XAUTHORITY']
    for name in passthrough_env_names:
        if name in os.environ:
            interactive_env[name] = os.environ[name]

    # Override shell prompt to ease confusion
    # \w is the "current working directory"
    git_tag_command = 'git describe --abbrev=0 --tag 2>/dev/null'
    interactive_env['PS1'] = f'\n{purple}[interactive]{reset} {green}\\w{reset} {turquoise}($({git_tag_command})){reset}\n\\$ '

    print()
    print('Spawning a new shell, type `exit` to leave.')
    print()
    print('You can run `./do-<stage>.sh` to run configure, build, install, or test.')
    print('You can run `source setup.sh` to use the installation in `install/usr/local/mdsplus`.')

    # --login and --noprofile allow for $PS1 to be set and not overwritten
    subprocess.run(
        [ shell, '--login', '--noprofile' ],
        cwd=args.workspace,
        env=interactive_env,
    )

def do_setup_vscode():
    global source_dir, build_dir
    
    # Force a reconfigure to generate launch.json targets
    args.configure = True
    cmake_args.append('-DGENERATE_VSCODE_LAUNCH_JSON=ON')
    # See do_generate_vscode_launch_json() for the running of the 'generate-vscode-launch-json' target

    # Update .vscode/settings.json

    vscode_directory = os.path.join(source_dir, '.vscode/')
    vscode_settings_filename = os.path.join(vscode_directory, 'settings.json')

    os.makedirs(vscode_directory, exist_ok=True)

    vscode_settings = {}
    if os.path.exists(vscode_settings_filename):
        with open(vscode_settings_filename, 'rt') as file:
            try:
                vscode_settings = json.load(file)
            except:
                pass

    # Configure the CMake plugin to find our build directory
    vscode_settings['cmake.buildDirectory'] = build_dir

    # Disable the C/C++ plugin's intellisense
    vscode_settings['C_Cpp.intelliSenseEngine'] = 'disabled'

    clangd_argument_map = {}

    if 'clangd.arguments' in vscode_settings:
        for argument in vscode_settings['clangd.arguments']:
            key, value = argument.split('=', maxsplit=1)
            clangd_argument_map[key] = value

    # Disable clangd's terrible automatic #include insertion
    clangd_argument_map['--header-insertion'] = 'never'

    # Configure clangd to find our compile_commands.json
    clangd_argument_map['--compile-commands-dir'] = build_dir

    vscode_settings['clangd.arguments'] = [ f'{k}={v}' for k, v in clangd_argument_map.items() ]

    with open(vscode_settings_filename, 'wt') as file:
        json.dump(vscode_settings, file, indent=4)
    
    import atexit
    atexit.register(print, '\nVisual Studio Code Settings Configured, Run "clangd: Restart language server" to apply')

def do_configure():
    global cmake_args, cmake_cache, cmake, source_dir, build_dir

    os.makedirs(build_dir, exist_ok=True)

    # If we have not already configured
    if 'CMAKE_GENERATOR' not in cmake_cache:
        # And the user has not specified a generator
        if not any(arg.startswith('-G') for arg in cmake_args):
            # Try to use Ninja if it is available
            ninja = shutil.which('ninja')
            if ninja is not None:
                cmake_args.append('-GNinja')

    print('CMake arguments:')
    for arg in cmake_args:
        print(f"    {arg}")
    print()

    print('Configuring')
    result = subprocess.run(
        [ cmake, source_dir ] + cmake_args,
        cwd=build_dir,
    )

    if result.returncode != 0:
        print('--configure failed')
        exit(1)

    cmake_cache = parse_cmake_cache()

def do_build():
    global args, cmake_cache, build_dir

    os.makedirs(build_dir, exist_ok=True)

    build_command = [ cmake, '--build', build_dir ]
    build_tool_arguments = []

    # If we know the generator, we can pass arguments to the underlying build tool
    if 'CMAKE_GENERATOR' in cmake_cache:
        generator = cmake_cache['CMAKE_GENERATOR']

        if generator == 'Unix Makefiles' or generator == 'Ninja':
            build_tool_arguments.append(f'-j{args.parallel}')

    if args.clean:
        build_command.append('--clean-first')

    print(f"Building with {' '.join(build_command)} -- {' '.join(build_tool_arguments)}")
    result = subprocess.run(
        build_command + [ '--' ] + build_tool_arguments,
        cwd=build_dir,
    )

    if result.returncode != 0:
        print('--build failed')
        exit(1)

def do_generate_vscode_launch_json():
    global cmake, build_dir

    # Run the custom target to generate .vscode/launch.json
    result = subprocess.run(
        [ cmake, '--build', build_dir, '--target', 'generate-vscode-launch-json' ],
        cwd=build_dir,
    )
        
    if result.returncode != 0:
        print('--setup-vscode failed')
        exit(1)

def do_install():
    global args, cmake, build_dir, install_dir, usr_local_mdsplus_dir

    # The install directory is set during configure with -DCMAKE_INSTALL_PREFIX={usr_local_mdsplus_dir}
    os.makedirs(usr_local_mdsplus_dir, exist_ok=True)
    
    print('Installing')
    result = subprocess.run(
        # The . tells CMake where to find the configuration to use during installation, not where to install to
        [ cmake, '--install', '.' ],
        cwd=build_dir,
    )

    if result.returncode != 0:
        print('--install failed')
        exit(1)

def do_package():
    global args, cmake_cache, packages_dir, dist_dir

    os.makedirs(packages_dir, exist_ok=True)
    os.makedirs(dist_dir, exist_ok=True) # mdsplus.org/dist/{--distname}/
    
    print('Packaging')

    if args.distname is None:
        print('You must specify --distname when using --package')
        exit(1)
    
    if args.platform is None:
        print('Unable to autodetect --platform, manually specify --platform to use --package')
        exit(1)

    if args.flavor is None:
        args.flavor = cmake_cache.get('RELEASE_BRANCH', 'unknown')

    if args.version is None:
        args.version = cmake_cache.get('RELEASE_VERSION', '0.0.0')

    # HACK: Remove after merging cmake branch
    if args.flavor == 'cmake':
        args.flavor = 'alpha'

    bname = ''
    if args.flavor != 'stable':
        bname = f'-{args.flavor}'

    if args.arch is None:
        if args.platform == 'debian':
            result = subprocess.run(
                [ '/usr/bin/dpkg', '--print-architecture' ],
                stdout=subprocess.PIPE
            )
            args.arch = result.stdout.decode().strip()

        elif args.platform == 'redhat':
            result = subprocess.run(
                [ '/usr/bin/rpm', '-E', '%{_arch}' ],
                stdout=subprocess.PIPE
            )
            args.arch = result.stdout.decode().strip()

        elif args.platform.startswith('macosx'):
            result = subprocess.run(
                [ '/usr/bin/uname', '-m' ],
                stdout=subprocess.PIPE
            )
            args.arch = result.stdout.decode().strip()

    if args.arch is None:
        print('Unable to autodetect --arch, manually specify --arch to use --package')
        exit(1)

    # TODO: Replace these with standard arguments when the packaging scripts are rewritten
    package_env = dict(os.environ)
    package_env['srcdir'] = source_dir
    package_env['ARCH'] = args.arch
    package_env['DISTNAME'] = args.distname
    package_env['PLATFORM'] = args.platform
    package_env['BRANCH'] = args.flavor
    package_env['FLAVOR'] = args.flavor
    package_env['BNAME'] = bname
    package_env['RELEASE_VERSION'] = args.version
    package_env['BUILDROOT'] = install_dir
    package_env['DISTROOT'] = dist_dir

    publish_info = {
        'flavor': args.flavor,
        'arch': args.arch, # ?
        'version': args.version,
        'distname': args.distname,
        'platform': args.platform,
        'dockerimage': os.environ.get('DOCKERIMAGE', None), # HACK: To determine what docker image we are in to pass to publish.py
        'packages': [],
    }

    # TODO: Move
    import tarfile

    if args.platform == 'alpine':
        pass
    elif args.platform == 'debian':

        # TODO: Return the list of deb files so we don't have to guess
        result = subprocess.run(
            [ sys.executable, os.path.join(deploy_dir, 'packaging/debian/debian_build_debs.py') ],
            cwd=build_dir,
            env=package_env,
        )

        if result.returncode != 0:
            print('Failed to build debian packages')
            exit(1)

        deb_files = glob.glob(os.path.join(dist_dir, f'**/*{args.flavor}*_{args.version}*.deb'), recursive=True)

        for filename in deb_files:
            publish_info['packages'].append(os.path.relpath(filename, dist_dir))

        # TODO: Verify verify is working
        if args.verify_packages:
            for filename in deb_files:
                result = subprocess.run(
                    ['/usr/bin/dpkg', '-c', filename],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT
                )

                if result.returncode != 0:
                    print('Failed to enumerate deb package contents for', filename)
                    exit(1)

                # mdsplus-alpha-package_bin_1.2.3_amd64.deb -> package_bin
                reference_filename = os.path.basename(filename)
                reference_filename = reference_filename.replace(f'mdsplus{bname}-', '')
                reference_filename = reference_filename.replace(f'_{args.version}_{args.arch}.deb', '')

                if '_bin' in reference_filename:
                    reference_filename += f'.{args.arch}'
                else:
                    reference_filename += '.noarch'

                print('Verifying contents of', os.path.basename(filename), 'against', reference_filename)

                reference_filename = os.path.join(deploy_dir, 'packaging', args.platform, reference_filename)
                if not os.path.exists(reference_filename):
                    print('Skipping')
                    continue

                install_filenames = []
                reference_filenames = [ line.strip() for line in open(reference_filename).readlines() ]

                dpkg_contents = result.stdout.decode().splitlines()
                for install_filename in dpkg_contents:

                    if len(install_filename.rstrip()) == 0:
                        continue

                    # TODO: Harden
                    # e.g. -rw-r--r-- 1234/5678    12345 1970-01-01 00:00 ./usr/local/mdsplus/file
                    install_filename = './' + install_filename.split(' ./')[1]

                    # .e.g ./path/to/file -> otherFile
                    if '->' in install_filename:
                        install_filename = install_filename.split(' -> ')[0]
                    
                    # Ignore directories
                    if install_filename[-1] == '/':
                        continue
                    
                    install_filenames.append(install_filename.strip())
                
                for install_filename, reference_filename in zip(sorted(install_filenames), sorted(reference_filenames)):
                    if install_filename != reference_filename:
                        print(f'"{install_filename}" != "{reference_filename}"')
                        exit(1)
                    # else:
                    #     print(f'"{install_filename}" == "{reference_filename}"')

        package_filename = os.path.join(packages_dir, f"mdsplus_{args.flavor}_{args.version}_{args.distname}_{args.arch}_debs.tgz")
        print(f'Creating {package_filename}')
        
        package_file = tarfile.open(package_filename, 'w:gz')

        for filename in deb_files:
            package_file.add(filename, arcname=os.path.basename(filename))
            
        package_file.close()

    elif args.platform == 'redhat':

        os.makedirs(os.path.join(install_dir, 'etc/pki/rpm-gpg'), exist_ok=True)
        os.makedirs(os.path.join(install_dir, 'etc/yum.repos.d'), exist_ok=True)

        rpm_gpg_key_url = 'http://www.mdsplus.org/dist/RPM-GPG-KEY-MDSplus'
        rpm_gpg_key_filename = os.path.join(install_dir, 'etc/pki/rpm-gpg/RPM-GPG-KEY-MDSplus')

        try:
            request.urlretrieve(rpm_gpg_key_url, rpm_gpg_key_filename)
        except request.HTTPError:
            print(f'Failed to retrieve "{rpm_gpg_key_url}", unable to create repo package')
            exit(1)

        with open(os.path.join(install_dir, f'etc/yum.repos.d/mdsplus{bname}.repo'), 'wt') as file:
            repo_lines = [
                f'[MDSplus{bname}]',
                f'name=MDSplus{bname}',
                f'baseurl=http://www.mdsplus.org/dist/{args.distname}/{args.flavor}/RPMS',
                f'enabled=1',
                f'gpgcheck=1',
                f'repo_gpgcheck=1',
                f'gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-MDSplus',
                f'metadata_expire=300',
                '',
            ]
            file.write('\n'.join(repo_lines))

        result = subprocess.run(
            [ sys.executable, os.path.join(deploy_dir, 'packaging/redhat/redhat_build_rpms.py') ],
            cwd=build_dir,
            env=package_env,
        )

        if result.returncode != 0:
            print('Failed to build redhat packages')
            exit(1)

        redhat_package_version = '-'.join(args.version.rsplit('.', maxsplit=1)) # 1.2-3
        rpm_files = glob.glob(os.path.join(dist_dir, f'**/*{args.flavor}*-{redhat_package_version}*.rpm'), recursive=True)

        for filename in rpm_files:
            publish_info['packages'].append(os.path.relpath(filename, dist_dir))

        if args.verify_packages:
            for filename in rpm_files:
                result = subprocess.run(
                    [ '/bin/bash', '-c', f'rpm2cpio {filename} | cpio --list --quiet | sort' ],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                )

                if result.returncode != 0:
                    print('Failed to run list contents of', filename)
                    exit(1)

                # mdsplus-alpha-package_bin_1.2-3_x86_64.rpm -> package_bin
                reference_filename = os.path.basename(filename)
                reference_filename = reference_filename.replace(f'mdsplus{bname}-', '')
                reference_filename = reference_filename.replace(f'-{redhat_package_version}.{args.distname}', '')
                reference_filename = reference_filename.replace('.rpm', '')

                print('Verifying contents of', os.path.basename(filename), 'against', reference_filename)

                reference_filename = os.path.join(deploy_dir, 'packaging', args.platform, reference_filename)
                if not os.path.exists(reference_filename):
                    print('Skipping')
                    continue

                install_filenames = [ line.strip() for line in result.stdout.decode().splitlines() ]
                reference_filenames = [ line.strip() for line in open(reference_filename).readlines() ]
                
                for install_filename, reference_filename in zip(sorted(install_filenames), sorted(reference_filenames)):
                    if install_filename != reference_filename:
                        print(f'"{install_filename}" != "{reference_filename}"')
                        exit(1)
                    # else:
                    #     print(f'"{install_filename}" == "{reference_filename}"')

        package_filename = os.path.join(packages_dir, f"mdsplus_{args.flavor}_{args.version}_{args.distname}_{args.arch}_rpms.tgz")
        print(f'Creating {package_filename}')

        package_file = tarfile.open(package_filename, 'w:gz')

        for filename in rpm_files:
            package_file.add(filename, arcname=os.path.basename(filename))
            
        package_file.close()

    elif args.platform == 'windows':

        result = subprocess.run(
            [ os.path.join(deploy_dir, 'packaging/windows/create_installer.sh') ],
            cwd=build_dir,
            env=package_env,
        )

        if result.returncode != 0:
            print('Failed to build windows installer')
            exit(1)

        exe_version = '-'.join(args.version.rsplit('.', maxsplit=1)) # 1.2-3
        exe_list = glob.glob(os.path.join(dist_dir, f'**/*{args.flavor}-{exe_version}*.exe'), recursive=True)
        for filename in exe_list:
            publish_info['packages'].append(os.path.relpath(filename, dist_dir))
            shutil.copy(filename, packages_dir)

    publish_info_filename = os.path.join(args.workspace, 'mdsplus-publish.json')

    with open(publish_info_filename, 'wt') as file:
        file.write(json.dumps(publish_info, indent=2))

    root_package_filename = os.path.join(packages_dir, f"mdsplus_{args.flavor}_{args.version}_{args.distname}_{args.arch}.tgz")

    print(f'Creating {root_package_filename}')
    root_package_file = tarfile.open(root_package_filename, 'w:gz')
    # Put the files in the root of the tarfile, arcname is "alternative name in the archive"
    root_package_file.add(usr_local_mdsplus_dir, arcname='.')
    root_package_file.close()

def do_test():
    global args, build_dir, testing_dir

    os.makedirs(testing_dir, exist_ok=True)

    print('Testing')

    test_data_filename = os.path.join(args.workspace, 'mdsplus-test.json')

    result = subprocess.run(
        [ ctest, '-N', '--show-only=json-v1' ],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        cwd=build_dir,
    )

    if result.returncode != 0:
        print('Failed to retrieve list of available tests, have you built yet?')
        exit(1)

    test_queue = []
    test_data = json.loads(result.stdout.decode())

    for i, test in enumerate(test_data['tests']):
        test_queue.append({
            'index': i + 1,
            'name': test['name'],
        })

    start_time = datetime.now()
    total_time_test = 0
    test_count = len(test_queue)

    running_tests = []
    finished_tests = {}

    def stop_testing(signum, frame):
        # Clear the test queue
        test_queue.clear()

        # Kill all running tests
        for test in running_tests:
            print(f"Killing {test['name']}")
            test['process'].kill()

    signal.signal(signal.SIGINT, stop_testing)

    if args.rerun_failed:
        print('Re-Running failed tests')

        try:
            # Preemptively "pass" the tests that previously passed
            with open(test_data_filename, 'rt') as file:
                old_tests = json.loads(file.read())
                # We cannot modify a list while iterating over it, so use a copy
                for test in test_queue.copy():
                    if test['name'] in old_tests.keys():
                        old_test = old_tests[test['name']]
                        if old_test['passed']:
                            test_queue.remove(test)
                            finished_tests[test['name']] = old_test
        except:
            print(f'Failed to parse {test_data_filename}')
    
    test_regex = None
    if args.test_regex is not None:
        import re
        test_regex = re.compile(f".*{args.test_regex}.*")

    while len(test_queue) > 0 or len(running_tests) > 0:

        # Check on all running tests and remove completed ones from the list
        for test in running_tests:
            result = test['process'].poll()
            if result is not None:
                running_tests.remove(test)

                test['log_file'].close()

                delta_time = datetime.now() - test['start_time']
                delta_time = delta_time.total_seconds()
                total_time_test += delta_time

                passed = (result == 0)

                result_message = 'Success' if passed else 'Failed'
                print(f"[{test['index']:3}/{test_count}] {result_message}: {test['name']} ({delta_time:.3f}s)")
                if not passed:
                    print(f"[{test['index']:3}/{test_count}] Log File: {test['log_filename']}")

                test_record = {
                    'index': test['index'],
                    'log': test['log_filename'],
                    'time': delta_time,
                    'passed': passed,
                }

                finished_tests[test['name']] = test_record
        
        # Take tests from the queue, start them, and add them to running_tests
        while len(test_queue) > 0 and len(running_tests) < int(args.parallel):
            test = test_queue.pop(0)

            if test_regex is not None:
                if test_regex.match(test['name']) is None:
                    print(f"Skipping: {test['name']}, does not match the --test-regex")
                    continue
            
            log_filename = os.path.join(testing_dir, f"{test['name']}.log")
            os.makedirs(os.path.dirname(log_filename), exist_ok=True)
            log_file = open(log_filename, 'wb')

            print(f"[{test['index']:3}/{test_count}] Running: {test['name']}")

            test_start_time = datetime.now()

            test_process = subprocess.Popen(
                [ ctest, '-I', f"{test['index']},{test['index']}", '-V' ],
                stdout=log_file,
                stderr=subprocess.STDOUT,
                cwd=build_dir,
            )

            running_tests.append({
                'index': test['index'],
                'name': test['name'],
                'process': test_process,
                'start_time': test_start_time,
                'log_filename': log_filename,
                'log_file': log_file,
            })

    signal.signal(signal.SIGINT, signal.SIG_DFL)

    passed_test_count = 0
    failed_test_count = 0
    for name, test in finished_tests.items():
        if test['passed']:
            passed_test_count += 1
        else:
            failed_test_count += 1

    percentage = 0
    if test_count > 0:
        percentage = math.floor((passed_test_count / test_count) * 100.0)
    
    total_time_real = datetime.now() - start_time
    total_time_real = total_time_real.total_seconds()

    print()
    print(f"{passed_test_count}/{test_count} tests passed, {percentage:.0f}%")
    print()
    print(f"Took {total_time_test:.3f}s (real {total_time_real:.3f}s)")
    print()

    with open(test_data_filename, 'wt') as file:
        file.write(json.dumps(finished_tests, indent=2))

    if failed_test_count > 0:
        print("The following tests failed:")

        for name, test in finished_tests.items():
            if not test['passed']:
                log_filename_escaped = test['log'].replace(' ', '\\ ')
                print(f"    #{test['index']} {name} ({log_filename_escaped})")

        print()
        print('You can run only these tests by passing --rerun-failed')
    
    if args.output_junit:
        import xml.etree.ElementTree as xml

        root = xml.Element('testsuites')
        root.attrib['time'] = str(total_time_test)
        root.attrib['tests'] = str(len(finished_tests))
        root.attrib['failures'] = str(failed_test_count)

        testsuite = xml.SubElement(root, 'testsuite')
        testsuite.attrib['time'] = str(total_time_test)
        testsuite.attrib['name'] = args.junit_suite_name

        for test_name, test in finished_tests.items():
            testcase = xml.SubElement(testsuite, 'testcase')
            testcase.attrib['name'] = test_name
            testcase.attrib['time'] = str(test['time'])

            system_out = xml.SubElement(testcase, 'system-out')
            system_out.text = open(test['log'], 'rt').read()
            
            # The BEL character causes issues when loaded into Jenkins
            system_out.text = system_out.text.replace('\x07', '')

            if not test['passed']:
                failure = xml.SubElement(testcase, 'failure')
                failure.attrib['message'] = 'Failed'

        junit_filename = os.path.join(args.workspace, 'mdsplus-junit.xml')
        print(f'Writing jUnit XML to {junit_filename}')
        with open(junit_filename, 'wb') as file:
            file.write(xml.tostring(root))

    if failed_test_count > 0:
        exit(1)

# main

if args.dockerimage is not None:
    do_docker()
else:

    # TODO: Move
    print()
    print('Combined build arguments:')
    for arg in build_command_line():
        print(f"    {arg}")
    print()

    if args.interactive:
        do_interactive()

    else:

        if args.setup_vscode:
            do_setup_vscode()

        if args.configure:
            do_configure()

        if args.build:
            do_build()

        if args.setup_vscode:
            do_generate_vscode_launch_json()

        if args.install:
            do_install()

        if args.package:
            do_package()

        if args.test:
            do_test()
