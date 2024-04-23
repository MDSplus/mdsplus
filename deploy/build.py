#!/usr/bin/env python3

import sys

if not sys.version_info >= (3, 6):
    print('This script must be run with python >= 3.6')
    exit(1)

import argparse
import json
import math
import os
import shutil
import signal
import subprocess
import platform
import glob

from datetime import datetime

deploy_dir = os.path.dirname(os.path.abspath(__file__))
root_dir = os.path.dirname(deploy_dir)

os_options = {}
for file in glob.glob(os.path.join(deploy_dir, 'os/*.opts')):
    name = os.path.basename(file).removesuffix('.opts')
    if os.path.islink(file):
        real_file = os.path.realpath(file)
        real_name = os.path.basename(real_file).removesuffix('.opts')

        if real_name not in os_options:
            os_options[real_name] = []

        os_options[real_name].append(name)
    else:
        if name not in os_options:
            os_options[name] = []

os_option_text = 'The following values (and aliases) for --os are available:\n'
for name, aliases in sorted(os_options.items()):
    os_option_text += f'    {name}'
    if len(aliases) > 0:
        os_option_text += f" -> {', '.join(sorted(aliases, reverse=True))}"
    os_option_text += '\n'

parser = argparse.ArgumentParser(
    formatter_class=argparse.RawDescriptionHelpFormatter,
    epilog=os_option_text,
)

parser.add_argument(
    '--os',
    metavar='',
    help='The OS definition to use (see below), which will reference `deploy/os/{--os}.opts` for additional parameters to this script and `deploy/os/{--os}.env` for additional environment variables. This will also change the default for --workspace to be `workspace-{--os}/`',
)

parser.add_argument(
    '--test',
    action='store_true',
    # python 3.9+
    # action=argparse.BooleanOptionalAction,
    help='',
)

parser.add_argument(
    '--no-test',
    action='store_true',
    help='',
)

parser.add_argument(
    '-G', '--generator',
    metavar='',
    help='CMake Generator, defaults to "Unix Makefiles", see `cmake --help` for more options.',
)

parser.add_argument(
    '-i', '--interactive',
    action='store_true',
    help='Drop into an interactive shell, allowing you to configure/build/install/test. This will attempt to clean your environment of references to $MDSPLUS_DIR if any are found.',
)

parser.add_argument(
    '-j', '--parallel',
    nargs='?',
    const=os.cpu_count(),
    default=1,
    metavar='',
    help='The number of parallel files to build or tests to run, defaults to `nproc` if no value is specified.',
)

parser.add_argument(
    '--env-file',
    metavar='',
    help='A shell script that will be used to determine the additional environment to add while running. This is to `deploy/os/{--os}.env` if --os is set and the file exists.',
)

parser.add_argument(
    '--workspace',
    metavar='',
    help='The directory that will contain the default build/install directories and helper scripts, defaults to `workspace/` or `workspace-{--os}/` if --os is specified.',
)

parser.add_argument(
    '--source',
    metavar='',
    help='The directory containing the source files, used for CMAKE_SOURCE_DIR, defaults to the directory above this file.',
)

parser.add_argument(
    '--buildroot',
    metavar='',
    help='The directory that will contain the intermediate files, used for CMAKE_BINARY_DIR, defaults to `{--workspace}/build/`.',
)

parser.add_argument(
    '--install',
    metavar='',
    help='The directory that will contain the full usr/local/mdsplus installation, used for CMAKE_INSTALL_PREFIX, defaults to `{--workspace}/install/`.',
    nargs='?',
    const=True,
    default=True,
)

parser.add_argument(
    '--dockerpull',
    action='store_true',
    help='Pull the latest docker image before creating the container.',
)

parser.add_argument(
    '--dockerimage',
    metavar='',
    help='Create a docker container with this image, and run the build inside there. Can be combined with -i/--interactive to get a shell inside the docker container.',
)

parser.add_argument(
    '--dockernetwork',
    metavar='',
    help='Create and use this docker network when creating the docker container.',
)

parser.add_argument(
    '--platform',
    metavar='',
    help='',
)

parser.add_argument(
    '--distname',
    metavar='',
    help='',
)

parser.add_argument(
    '--arch',
    metavar='',
    help='',
)

parser.add_argument(
    '--valgrind',
    metavar='',
    help='',
    nargs='?',
    const=True,
)

parser.add_argument(
    '--sanitize',
    metavar='',
    help='',
)

parser.add_argument(
    '--rerun-failed',
    metavar='',
    help='',
    nargs='?',
    const=True,
)

parser.add_argument(
    '--output-junit',
    metavar='',
    help='',
    nargs='?',
    const=True,
)

parser.add_argument(
    '--junit-suite-name',
    metavar='',
    help='',
)

###
### Helper functions
###

def build_command_line(args):
    build_args = []
    for name, value in args.items():
        if value is not None:
            if type(value) is bool:
                if value:
                    build_args.append(f'--{name}')
            else:
                build_args.append(f'--{name}={value}')
    return build_args

def parse_cmake_cache(filename):
    settings = {}
    try:
        with open(filename, 'rt') as f:
            for line in f.readlines():
                if line.startswith('//'):
                    continue
                try:
                    key, value = line.strip().split('=')
                    name, type = key.split(':')
                    settings[name] = value
                except:
                    continue
    except FileNotFoundError:
        pass
    return settings

###
### Parse arguments from the command line and deploy/os/{os}.opts, if --os= is set
###

cli_args, unknown_args = parser.parse_known_args()
cmake_args = unknown_args

if cli_args.os is not None:

    opts_filename = os.path.join(deploy_dir, f'os/{cli_args.os}.opts')

    if not os.path.exists(opts_filename):
        print(f'Unsupported --os={cli_args.os}, ensure that deploy/os/{cli_args.os}.opts exists')
        exit(1)

    os_alias = None
    if os.path.islink(opts_filename):
        opts_filename = os.path.realpath(opts_filename)
        os_alias = os.path.basename(opts_filename).removesuffix('.opts')
    
    opts = open(opts_filename).read().strip().split()

    env_filename = os.path.join(deploy_dir, f'os/{cli_args.os}.env')
    if os.path.exists(env_filename):
        opts.append(f'--env-file={env_filename}')
    else:
        env_filename = os.path.join(deploy_dir, f'os/{cli_args.os}-{platform.machine()}.env')
        if os.path.exists(opts_filename):
            opts.append(f'--env-file={env_filename}')

    cli_args, unknown_args = parser.parse_known_args(args=opts)
    cmake_args = unknown_args

    # To allow command-line arguments to override .opts file arguments, we need to parse them after the .opts file
    cli_args, unknown_args = parser.parse_known_args(namespace=cli_args)
    cmake_args.extend(unknown_args)

    if os_alias is not None:
        print()
        print(f"Using aliased --os={cli_args.os} -> --os={os_alias}")
        cli_args.os = os_alias

###
### Convert the arguments to a dictionary and fill in computed values
###

args = dict()

for name in vars(cli_args):
    # argparse replaces - with _ in the names
    original_name = name.replace('_', '-')
    args[original_name] = getattr(cli_args, name)

# TODO: Remove and replace with argparse.BooleanOptionalAction
if args['no-test']:
    args['test'] = False

if args['valgrind'] == '':
    args['valgrind'] = None

if args['sanitize'] == '':
    args['sanitize'] = None

if args['output-junit']:
    if args['junit-suite-name'] is None:
        args['junit-suite-name'] = args['os']

if args['dockernetwork'] == '':
    args['dockernetwork'] = None

if args['source'] is None:
    args['source'] = root_dir

if args['workspace'] is None:
    if args['os'] is None:
        args['workspace'] = 'workspace/'
    else:
        args['workspace'] = f"workspace-{args['os']}/"

# Convert the workspace to an absolute path
if not os.path.isabs(args['workspace']):
    args['workspace'] = os.path.join(root_dir, args['workspace'])

os.makedirs(args['workspace'], exist_ok=True)

if not args['buildroot']:
    args['buildroot'] = os.path.join(args['workspace'], 'build')

os.makedirs(args['buildroot'], exist_ok=True)

# If no install path is specified, use the default
if args['install'] == True:
    args['install'] = os.path.join(args['workspace'], 'install/usr/local/mdsplus')

os.makedirs(args['install'], exist_ok=True)

if args['dockerimage'] is not None:

    passthrough_args = []
    for arg in build_command_line(args):

        # We don't want the .opts files to be parsed recursively
        if arg.startswith('--os='):
            continue

        # We don't want docker to run recursively
        if arg.startswith('--docker'):
            continue

        passthrough_args.append(arg)

    if args['dockerpull']:
        # TODO: Make this visible to the user
        subprocess.run([ 'docker', 'pull', args['dockerimage'] ])

    docker_command = f"python3 {args['source']}/deploy/build.py {' '.join(passthrough_args)} {' '.join(cmake_args)}"

    # TODO: Improve
    # import platform
    # if platform.system() != 'Windows':
        # docker_command = f'groupadd -g {os.getgid()} build-group;' + f'useradd -u {os.getuid()} -g build-group -s /bin/bash -d /workspace build-user;' + 'exec su build-user -c "' + docker_command  + '"'
    
    docker_args = [
        f"--volume={args['workspace']}:{args['workspace']}",
        f"--volume={args['source']}:{args['source']}", # TODO: Improve?
        f"--volume={args['buildroot']}:{args['buildroot']}",
        f"--volume={args['install']}:{args['install']}",
        f"--workdir={args['workspace']}",
        args['dockerimage'],
    ]

    print()
    print('Docker arguments:')
    for arg in docker_args:
        print(f"    {arg}")
    print()

    docker_args.extend([
        '/bin/bash', '-c', docker_command
    ])

    # groupadd -g os.getgid() group_name
    # useradd -u os.getuid() -g group_name -h /workspace user_name

    import platform
    if platform.system() != 'Windows':
        docker_args = [ '--user', f"{os.getuid()}:{os.getgid()}" ] + docker_args

    if args['dockernetwork'] is not None:
        subprocess.run([ 'docker', 'network', 'create', args['dockernetwork'] ])
        # TODO: error checking
        docker_args = [ '--network', args['dockernetwork'] ] + docker_args

    if args['interactive']:
        subprocess.run(
            [
                'docker', 'run',
                '--interactive',
                '--tty',
                '--rm',
            ] + docker_args
        )
    else:
        result = subprocess.run(
            [
                'docker', 'run',
                '--detach',
                '--tty',
            ] + docker_args,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        if result.returncode != 0:
            print(f'Failed to run docker container: {result.stderr.decode()}')
            exit(1)
        
        container_id = result.stdout.decode().strip()

        docker_log = subprocess.Popen(
            [
                'docker', 'logs',
                '--follow',
                '--timestamps',
                container_id
            ],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        def kill_docker(signum, frame):
            print(f'\nKilling docker container {container_id}')
            
            subprocess.run([ 'docker', 'kill', container_id ])
            subprocess.run([ 'docker', 'rm', container_id ])

            if args['dockernetwork'] is not None:
                subprocess.run([ 'docker', 'network', 'rm', args['dockernetwork'] ])

            exit(1)

        signal.signal(signal.SIGINT, kill_docker)

        while True:
            line = docker_log.stdout.readline()
            if not line:
                break

            print(line.decode().rstrip())

        signal.signal(signal.SIGINT, signal.SIG_DFL)

        result = subprocess.run(
            [
                'docker',
                'inspect',
                container_id,
                '--format="{{.State.ExitCode}}"'
            ],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        # TODO: Improve
        exit_code = int(result.stdout.decode().strip().strip('"'))
        
        subprocess.run([ 'docker', 'rm', container_id ])

        if args['dockernetwork'] is not None:
            subprocess.run([ 'docker', 'network', 'rm', args['dockernetwork'] ])

        if exit_code != 0:
            exit(exit_code)

else:

    shell = '/bin/bash'
    if 'SHELL' in os.environ:
        shell = os.environ['SHELL']

    if args['env-file'] and os.path.exists(args['env-file']):
        result = subprocess.run(
            [ shell, '-c', f". {args['env-file']}; env" ],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        if result.returncode != 0:
            print(f"Failed to source {args['env-file']}: {result.stderr.decode()}")
            exit(1)

        new_environ = {}
        for line in result.stdout.decode().splitlines():
            name, value = line.split('=', maxsplit=1)
            if name == '_':
                continue
            new_environ[name] = value
        
        env_changes = new_environ.copy()
        for name, value in new_environ.items():
            if name in os.environ:
                if new_environ[name] == os.environ[name]:
                    del env_changes[name]

        os.environ.update(env_changes)

        print('Sourced the following environment variables:')
        for name, value in env_changes.items():
            print(f'    {name}: {value}')
        print()

    # TODO: Improve
    cmake = shutil.which('cmake')
    if cmake is None:
        print('Unable to find cmake')
        exit(1)

    ctest = shutil.which('ctest')
    if ctest is None:
        print('Unable to find ctest')
        exit(1)

    # TODO: Improve
    cmake_cache_filename = os.path.join(args['buildroot'], 'CMakeCache.txt')
    cmake_cache = parse_cmake_cache(cmake_cache_filename)

    current_generator = None
    if 'CMAKE_GENERATOR' in cmake_cache:
        current_generator = cmake_cache['CMAKE_GENERATOR']
        
    if args['generator'] is None:
        args['generator'] = current_generator
    
    if args['generator'] != current_generator:
        # Perform a clean configure
        cmake_args.append('--fresh')

    if args['generator'] is None:
        args['generator'] = 'Unix Makefiles'

    # TODO: Add more
    build_command = f'{cmake} --build .'
    if args['generator'] == 'Unix Makefiles':
        build_command = f"make -j{args['parallel']}"
    elif args['generator'] == 'Ninja':
        build_command = f"ninja -j{args['parallel']}"

    if args['install'] is not None:
        install_command = f"{build_command} install"

    cmake_args = [ '-G', args['generator'] ] + cmake_args

    if args['sanitize'] is not None and args['valgrind'] is not None:
        print()
        print('It is not recommended to run valgrind with a sanitizer')
        print()

    if args['sanitize'] is not None:
        flavor = args['sanitize']
        cmake_args.append(f'-DENABLE_SANITIZE={flavor}')

    if args['valgrind'] is not None:
        cmake_args.append('-DENABLE_VALGRIND=ON')

        if args['valgrind'] != True:
            cmake_args.append(f"-DVALGRIND_TOOLS={args['valgrind']}")
    else:
        cmake_args.append('-DENABLE_VALGRIND=OFF')

    print()
    print('Combined build arguments:')
    for arg in build_command_line(args):
        print(f"    {arg}")
    print()

    if args['test']:
        cmake_args.append('-DBUILD_TESTING=ON')

    if args['install'] is not None:
        cmake_args.append(f"-DCMAKE_INSTALL_PREFIX={args['install']}")

    print('CMake arguments:')
    for arg in cmake_args:
        print(f"    {arg}")
    print()

    if args['interactive']:

        # TODO: Improve
        cmake_args_env = ''
        for arg in cmake_args:
            if ' ' in arg:
                cmake_args_env += f'\'{arg}\' '
            else:
                cmake_args_env += f'{arg} '
                

        configure_filename = os.path.join(args['workspace'], 'do-configure.sh')
        with open(configure_filename, 'wt') as file:
            file.write('#!/bin/bash\n')
            file.write(f"cd {args['buildroot']}\n")
            file.write(f"{cmake}  {cmake_args_env} \"$@\" {args['source']}\n")

        os.chmod(configure_filename, 0o755)

        build_filename = os.path.join(args['workspace'], 'do-build.sh')
        with open(build_filename, 'wt') as file:
            file.write('#!/bin/bash\n')
            file.write(f"cd {args['buildroot']}\n")
            file.write(f"{build_command} \"$@\"\n")
            
        os.chmod(build_filename, 0o755)

        if args['install'] is not None:
            install_filename = os.path.join(args['workspace'], 'do-install.sh')
            with open(install_filename, 'wt') as file:
                file.write('#!/bin/bash\n')
                file.write(f"cd {args['buildroot']}\n")
                file.write(f"{install_command} \"$@\"\n")

            os.chmod(install_filename, 0o755)

            setup_filename = os.path.join(args['workspace'], 'setup.sh')
            with open(setup_filename, 'wt') as file:
                file.write('#!/bin/bash\n')
                file.write(f"export MDSPLUS_DIR={args['install']}\n")
                file.write(f"source $MDSPLUS_DIR/setup.sh\n")
                
            os.chmod(setup_filename, 0o755)


        # TODO: Check file locations as well?
        # TODO: Better handle environments where setup.sh is automatically sourced
        # TODO: Python?
        if 'MDSPLUS_DIR' in os.environ:
            mdsplus_dir = os.environ['MDSPLUS_DIR']

            print('Detected $MDSPLUS_DIR, cleaning environment variables to allow sourcing setup.sh')

            modified_envs = []

            for key, value in os.environ.items():
                if mdsplus_dir in value:

                    modified_envs.append(f"${key}")

                    sep = os.pathsep
                    if key == 'MDS_PATH' or key.endswith('_path'):
                        sep = ';'

                    parts = []
                    for part in value.split(sep):
                        if mdsplus_dir not in part:
                            parts.append(part)

                    if len(parts) > 0:
                        os.environ[key] = sep.join(parts)
                    else:
                        del os.environ[key]

            modified_envs = ', '.join(modified_envs)
            print(f"    Modified: {modified_envs}")
            print()

            # For systems where setup.sh is automatically sourced
            os.environ['MDSPLUS_DIR'] = args['install']
            
        print('Spawning a new shell, type `exit` to leave.')
        print()

        subprocess.run(
            [ shell ],
            cwd=args['workspace'],
        )
    
    else:
        
        result = subprocess.run(
            [ cmake ] + cmake_args + [ args['source'] ],
            cwd=args['buildroot'],
        )

        if result.returncode != 0:
            print('CMake Configure failed')
            exit(result.returncode)

        result = subprocess.run(
            [
                shell, '-c',
                build_command
            ],
            cwd=args['buildroot'],
        )

        if result.returncode != 0:
            print('CMake Build failed')
            exit(result.returncode)

        if args['install'] is not None:
            subprocess.run(
                [
                    shell, '-c',
                    install_command
                ],
                cwd=args['buildroot'],
            )

            fake_setup_filename = os.path.join(args['workspace'], 'setup.sh')
            with open(fake_setup_filename, 'wt') as file:
                file.write('#!/bin/sh\n')
                file.write(f"export MDSPLUS_DIR=\"{args['install']}\"\n")
                file.write('. $MDSPLUS_DIR/setup.sh\n')

            # todo: error checking

        if args['test']:
            # You can run ctest -j to run tests in parallel, but in order to capture the output in log files
            # we iterate over the tests manually.

            TEST_DATA_FILENAME = os.path.join(args['workspace'], 'mdsplus-test.json')
            JUNIT_FILENAME = os.path.join(args['workspace'], 'mdsplus-junit.xml')

            result = subprocess.run(
                [
                    ctest, '-N', '--show-only=json-v1'
                ],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                cwd=args['buildroot'],
            )

            if result.returncode != 0:
                # TODO:
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

            passed_tests = {}
            failed_tests = {}
            running_test_list = []

            def kill_test_processes(signum, frame):
                for test in running_test_list:
                    print(f"Killing {test['name']}")
                    test['process'].kill()

                test_queue.clear()

            signal.signal(signal.SIGINT, kill_test_processes)

            if args['rerun-failed']:
                print('Re-Running failed tests')

                try:
                    with open(TEST_DATA_FILENAME, 'rt') as f:
                        old_tests = json.loads(f.read())
                        for test in test_queue.copy():
                            if test['name'] in old_tests.keys():
                                old_test = old_tests[test['name']]
                                if old_test['passed']:
                                    print(f"Skipping: {test['name']}")
                                    test_queue.remove(test)
                                    passed_tests[test['name']] = old_test
                except:
                    print(f'Failed to parse {TEST_DATA_FILENAME}')

            while len(test_queue) > 0 or len(running_test_list) > 0:

                for test in running_test_list:
                    result = test['process'].poll()
                    if result is not None:
                        running_test_list.remove(test)

                        delta_time = datetime.now() - test['start']
                        delta_time = delta_time.total_seconds()
                        total_time_test += delta_time

                        passed = (result == 0)

                        result_message = 'Success' if passed else 'Failed'
                        print(f"[{test['index']:3}/{test_count}] {result_message}: {test['name']} ({delta_time:.3f}s)")
                        if result != 0:
                            print(f"[{test['index']:3}/{test_count}] Log File: {test['log']}")

                        test_record = {
                            'index': test['index'],
                            'log': test['log'],
                            'time': delta_time,
                            'passed': passed,
                        }

                        if result == 0:
                            passed_tests[test['name']] = test_record
                        else:
                            failed_tests[test['name']] = test_record

                while len(test_queue) > 0 and len(running_test_list) < int(args['parallel']):
                    test = test_queue.pop(0)

                    log_filename = os.path.join(args['buildroot'], 'testing', test['name'] + '.log')
                    os.makedirs(os.path.dirname(log_filename), exist_ok=True)
                    log_file = open(log_filename, 'wb')

                    print(f"[{test['index']:3}/{test_count}] Running: {test['name']}")

                    test_start_time = datetime.now()

                    test_env = dict(os.environ)
                    test_env['mdsevent_port'] = ''
                    
                    test_process = subprocess.Popen(
                        [ ctest, '-I', f"{test['index']},{test['index']}", '-V' ],
                        stdout=log_file,
                        stderr=subprocess.STDOUT,
                        cwd=args['buildroot'],
                        env=test_env,
                    )
                    
                    running_test_list.append({
                        'index': test['index'],
                        'start': test_start_time,
                        'name': test['name'],
                        'process': test_process,
                        'log': log_filename,
                    })

            signal.signal(signal.SIGINT, signal.SIG_DFL)

            passed_test_count = len(passed_tests)

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

            all_tests = dict(passed_tests, **failed_tests)

            with open(TEST_DATA_FILENAME, 'wt') as f:
                f.write(json.dumps(
                    all_tests,
                    indent=2
                ))

            if len(failed_tests) > 0:
                print("The following tests failed:")

                for name, test in failed_tests.items():
                    log_filename_escaped = test['log'].replace(' ', '\\ ')
                    print(f"    {test['index']} {name} ({log_filename_escaped})")

            if args['output-junit']:
                import xml.etree.ElementTree as xml

                root = xml.Element('testsuites')
                root.attrib['time'] = str(total_time_test)
                root.attrib['tests'] = str(len(all_tests))
                root.attrib['failures'] = str(len(failed_tests))

                testsuite = xml.SubElement(root, 'testsuite')
                testsuite.attrib['time'] = str(total_time_test)

                # TODO: Improve
                testsuite.attrib['name'] = 'mdsplus'
                if args['junit-suite-name'] is not None:
                    testsuite.attrib['name'] = args['junit-suite-name']

                for test_name, test in all_tests.items():
                    testcase = xml.SubElement(testsuite, 'testcase')
                    testcase.attrib['name'] = test_name
                    testcase.attrib['time'] = str(test['time'])

                    system_out = xml.SubElement(testcase, 'system-out')
                    system_out.text = open(test['log']).read()

                    if not test['passed']:
                        failure = xml.SubElement(testcase, 'failure')
                        failure.attrib['message'] = 'Failed'
                
                print(f'Writing JUnit output to {JUNIT_FILENAME}')
                with open(JUNIT_FILENAME, 'wb') as f:
                    f.write(xml.tostring(root))
            
            # Report the failure to Jenkins
            if len(failed_tests) > 0:
                exit(1)