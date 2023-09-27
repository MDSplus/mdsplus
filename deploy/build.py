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

from datetime import datetime

deploy_dir = os.path.dirname(os.path.abspath(__file__))
root_dir = os.path.dirname(deploy_dir)

parser = argparse.ArgumentParser()

parser.add_argument(
    '--os',
    metavar='',
    help='',
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
    '-i', '--interactive',
    action='store_true',
    help='',
)

nproc = os.cpu_count()

parser.add_argument(
    '-j', '--parallel',
    nargs='?',
    const=nproc,
    default=1,
    metavar='',
    help='',
)

parser.add_argument(
    '--source',
    metavar='',
    help='',
)

parser.add_argument(
    '-G', '--generator',
    metavar='',
    help='',
)

parser.add_argument(
    '--env-file',
    metavar='',
    help='',
)

parser.add_argument(
    '--workspace',
    metavar='',
    help='',
)

parser.add_argument(
    '--dockerpull',
    action='store_true',
    help='',
)

parser.add_argument(
    '--dockerimage',
    metavar='',
    help='',
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
    nargs='?',
    const=True,
)

parser.add_argument(
    '--rerun-failed',
    metavar='',
    help='',
    nargs='?',
    const=True,
)

args, unknown_args = parser.parse_known_args()
cmake_args = unknown_args

if args.os is not None:

    opts_filename = os.path.join(deploy_dir, f'os/{args.os}.opts')
    if not os.path.exists(opts_filename):
        print(f'Unsupported --os={args.os}, ensure that deploy/os/{args.os}.opts exists')
        exit(1)
    
    opts = open(opts_filename).read().strip().split()

    env_filename = os.path.join(deploy_dir, f'os/{args.os}.env')
    if os.path.exists(env_filename):
        opts.append(f'--env-file={env_filename}')

    args, unknown_args = parser.parse_known_args(args=opts)
    cmake_args = unknown_args

    # To allow command-line arguments to override .opts file arguments, we need to parse them after the .opts file
    args, unknown_args = parser.parse_known_args(namespace=args)
    cmake_args.extend(unknown_args)

if args.source is None:
    args.source = root_dir

if args.workspace is None:
    if args.os is None:
        args.workspace = 'build/'
    else:
        branch = 'cmake' # TODO:
        args.workspace = f'build/{args.os}/{branch}'

# TODO: Remove and replace with argparse.BooleanOptionalAction
if args.no_test:
    args.test = False

# Convert the workspace to an absolute path
if not os.path.isabs(args.workspace):
    args.workspace = os.path.join(root_dir, args.workspace)

os.makedirs(args.workspace, exist_ok=True)

build_args = []
for name in vars(args):
    value = getattr(args, name)

    # argparse replaces - with _ in the names
    name = name.replace('_', '-')

    if value is not None:
        if type(value) is bool:
            if value:
                build_args.append(f'--{name}')
        else:
            build_args.append(f'--{name}={value}')

if args.dockerimage is not None:

    passthrough_args = build_args.copy()
    for name in passthrough_args:
        # Remove options that aren't applicable in the docker container
        if name.startswith('--os') or name.startswith('--docker'):
            passthrough_args.remove(name)

    if args.dockerpull:
        subprocess.run([ 'docker', 'pull', args.dockerimage ])

    docker_command = f"python3 {args.source}/deploy/build.py {' '.join(passthrough_args)} {' '.join(cmake_args)}"

    # TODO: Improve
    # import platform
    # if platform.system() != 'Windows':
        # docker_command = f'groupadd -g {os.getgid()} build-group;' + f'useradd -u {os.getuid()} -g build-group -s /bin/bash -d /workspace build-user;' + 'exec su build-user -c "' + docker_command  + '"'
    
    docker_args = [
        '--rm',
        '--volume', f'{root_dir}:{args.source}', # TODO: Improve?
        '--volume', f'{args.workspace}:/workspace',
        '--workdir', '/workspace',
        args.dockerimage,
        '/bin/bash', '-c', docker_command
    ]

    print(' '.join(docker_args))

    # groupadd -g os.getgid() group_name
    # useradd -u os.getuid() -g group_name -h /workspace user_name

    import platform
    if platform.system() != 'Windows':
        docker_args = [ '--user', f"{os.getuid()}:{os.getgid()}" ] + docker_args

    if args.interactive:
        subprocess.run(
            [
                'docker', 'run',
                '--interactive',
                '--tty',
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

        signal.signal(signal.SIGINT, kill_docker)

        while True:
            line = docker_log.stdout.readline()
            if not line:
                break

            print(line.decode().rstrip())

        signal.signal(signal.SIGINT, signal.SIG_DFL)

else:

    shell = '/bin/bash'
    if 'SHELL' in os.environ:
        shell = os.environ['SHELL']

    if args.env_file is not None:
        result = subprocess.run(
            [ shell, '-c', f'. {args.env_file}; env' ],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        if result.returncode != 0:
            print(f'Failed to source {args.env_file}: {result.stderr.decode()}')
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
    current_generator = None
    cmake_cache_filename = os.path.join(args.workspace, 'CMakeCache.txt')
    if os.path.exists(cmake_cache_filename):
       lines = open(cmake_cache_filename).readlines()
       for line in lines:
           if line.startswith('CMAKE_GENERATOR'):
               _, value = line.strip().split('=', maxsplit=1)
               current_generator = value
               break
           
    if args.generator is None:
        args.generator = current_generator
    
    if args.generator != current_generator:
        # Perform a clean configure
        cmake_args.append('--fresh')

    if args.generator is None:
        args.generator = 'Unix Makefiles'

    build_command = f'{cmake} --build .'
    if args.generator == 'Unix Makefiles':
        build_command = f'make -j{args.parallel}'
    elif args.generator == 'Ninja':
        build_command = f'ninja -j{args.parallel}'

    cmake_args = [ '-G', args.generator ] + cmake_args

    print()
    print('Combined build arguments:')
    print(f"    {' '.join(build_args)}")
    print()

    if args.test:
        cmake_args.append('-DBUILD_TESTING=ON')

    if args.valgrind == '':
        args.valgrind = None

    if args.sanitize == '':
        args.sanitize = None

    if args.sanitize is not None:
        sanitize_list = args.sanitize.split(',')
        print(sanitize_list)

    if args.valgrind is not None:
        cmake_args.append('-DENABLE_VALGRIND=ON')

        if args.valgrind != True:
            cmake_args.append(f'-DVALGRIND_TOOLS={args.valgrind}')
    else:
        cmake_args.append('-DENABLE_VALGRIND=OFF')

    print('CMake arguments:')
    print(f"    {' '.join(cmake_args)}")
    print()
    
    subprocess.run(
        [ cmake ] + cmake_args + [ args.source ],
        cwd=args.workspace,
    )

    if args.interactive:
        extra_env = {
            'SOURCE': args.source,
            'WORKSPACE': args.workspace,
        }

        print()
        print('The following environment variables have been set for interactive mode:')
        for name, value in extra_env.items():
            print(f'    {name}: {value}')
        print()
        print(f'To build, run: {build_command}')
        print('When you are done, run exit')
        print()

        subprocess.run(
            [ shell ],
            cwd=args.workspace,
        )
    else:
        subprocess.run(
            [
                shell, '-c',
                build_command
            ],
            cwd=args.workspace,
        )

        if args.test:
            # You can run ctest -j to run tests in parallel, but in order to capture the output in log files
            # we iterate over the tests manually.

            TEST_DATA_FILENAME = os.path.join(args.workspace, 'mdsplus-test.json')

            result = subprocess.run(
                [
                    ctest, '-N', '--show-only=json-v1'
                ],
                capture_output=True,
                cwd=args.workspace,
            )

            if result.returncode != 0:
                # TODO:
                exit(1)

            test_data = json.loads(result.stdout.decode())
            test_queue = []
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

            if args.rerun_failed:
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

                while len(test_queue) > 0 and len(running_test_list) < int(args.parallel):
                    test = test_queue.pop(0)

                    log_filename = os.path.join(args.workspace, 'testing', test['name'] + '.log')
                    os.makedirs(os.path.dirname(log_filename), exist_ok=True)
                    log_file = open(log_filename, 'wb')

                    print(f"[{test['index']:3}/{test_count}] Running: {test['name']}")

                    test_start_time = datetime.now()
                    
                    test_process = subprocess.Popen(
                        [ ctest, '-I', f"{test['index']},{test['index']}", '-V' ],
                        stdout=log_file,
                        stderr=subprocess.STDOUT,
                        cwd=args.workspace,
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
            percentage = math.floor((passed_test_count / test_count) * 100.0)

            total_time_real = datetime.now() - start_time
            total_time_real = total_time_real.total_seconds()

            print()
            print(f"{passed_test_count}/{test_count} tests passed, {percentage:.0f}%")
            print()
            print(f"Took {total_time_test:.3f}s (real {total_time_real:.3f}s)")
            print()

            if len(failed_tests) > 0:
                print("The following tests failed:")

                for name, test in failed_tests.items():
                    log_filename_escaped = test['log'].replace(' ', '\\ ')
                    print(f"    {test['index']} {name} ({log_filename_escaped})")

            with open(TEST_DATA_FILENAME, 'wt') as f:
                all_tests = dict(passed_tests, **failed_tests)
                f.write(json.dumps(
                    all_tests,
                    indent=2
                ))
            