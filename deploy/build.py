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

parser.add_argument(
    '--skip-default',
    metavar='',
    help='',
    nargs='?',
    const=True,
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
        opts_filename = os.path.join(deploy_dir, f'os/{cli_args.os}-{platform.machine()}.opts')
        if not os.path.exists(opts_filename):
            print(f'Unsupported --os={cli_args.os}, ensure that deploy/os/{cli_args.os}.opts exists')
            exit(1)
    
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

if args['source'] is None:
    args['source'] = root_dir

if args['workspace'] is None:
    if args['os'] is None:
        args['workspace'] = 'build/'
    else:
        branch = 'cmake' # TODO:
        args['workspace'] = f"build/{args['os']}/{branch}"

# Convert the workspace to an absolute path
if not os.path.isabs(args['workspace']):
    args['workspace'] = os.path.join(root_dir, args['workspace'])

os.makedirs(args['workspace'], exist_ok=True)

if args['dockerimage'] is not None:

    passthrough_args = build_command_line(args)
    for name in passthrough_args:
        # Remove options that aren't applicable in the docker container
        if name.startswith('--os') or name.startswith('--docker'):
            passthrough_args.remove(name)

    if args['dockerpull']:
        # TODO: Make this visible to the user
        subprocess.run([ 'docker', 'pull', args['dockerimage'] ])

    docker_command = f"python3 {args['source']}/deploy/build.py {' '.join(passthrough_args)} {' '.join(cmake_args)}"

    # TODO: Improve
    # import platform
    # if platform.system() != 'Windows':
        # docker_command = f'groupadd -g {os.getgid()} build-group;' + f'useradd -u {os.getuid()} -g build-group -s /bin/bash -d /workspace build-user;' + 'exec su build-user -c "' + docker_command  + '"'
    
    docker_args = [
        '--rm',
        '--volume', f"{root_dir}:{args['source']}", # TODO: Improve?
        '--volume', f"{args['workspace']}:/workspace",
        '--workdir', '/workspace',
        args['dockerimage'],
        '/bin/bash', '-c', docker_command
    ]

    print(' '.join(docker_args))

    # groupadd -g os.getgid() group_name
    # useradd -u os.getuid() -g group_name -h /workspace user_name

    import platform
    if platform.system() != 'Windows':
        docker_args = [ '--user', f"{os.getuid()}:{os.getgid()}" ] + docker_args

    if args['interactive']:
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

    if args['env-file'] is not None:
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

    if args['interactive']:
        
        extra_env = {
            'SOURCE': args['source'],
            'WORKSPACE': args['workspace'],
        }

        print()
        print('The following environment variables have been set for interactive mode:')
        for name, value in extra_env.items():
            print(f'    {name}: {value}')
        print()
        print('When you are done, run exit')
        print()

        env = dict(os.environ)
        env.update(extra_env)

        subprocess.run(
            [ shell ],
            cwd=args['workspace'],
            env=env
        )
    
    else:

        build_list = []

        if 'skip-default' in args and args['skip-default'] != True:
            default_build = {
                'name': 'default',
                'args': args.copy(),
                'cmake_args': cmake_args.copy(),
            }
            del default_build['args']['sanitize']

            build_list.append(default_build)

        sanitize_list = []
        if args['sanitize'] is not None:
            sanitize_list = args['sanitize'].split(',')

        # TODO: Inform the user that multiple builds are going to take place because of --sanitize
        
        for sanitizer in sanitize_list:
            build = {
                'name': f'sanitize-{sanitizer}',
                'args': args.copy(),
                'cmake_args': cmake_args.copy(),
            }

            build['args']['sanitize'] = sanitizer
            del build['args']['valgrind']

            build['cmake_args'].append(f'-DENABLE_SANITIZE={sanitizer}')
            build['args']['workspace'] = os.path.join(build['args']['workspace'], build['name'])

            os.makedirs(build['args']['workspace'], exist_ok=True)

            build_list.append(build)
        
        for build in build_list:

            # TODO: Improve
            cmake_cache_filename = os.path.join(build['args']['workspace'], 'CMakeCache.txt')
            cmake_cache = parse_cmake_cache(cmake_cache_filename)

            current_generator = None
            if 'CMAKE_GENERATOR' in cmake_cache:
                current_generator = cmake_cache['CMAKE_GENERATOR']
                
            if build['args']['generator'] is None:
                build['args']['generator'] = current_generator
            
            if build['args']['generator'] != current_generator:
                # Perform a clean configure
                build['cmake_args'].append('--fresh')

            if build['args']['generator'] is None:
                build['args']['generator'] = 'Unix Makefiles'

            # TODO: Add more
            build_command = f'{cmake} --build .'
            if build['args']['generator'] == 'Unix Makefiles':
                build_command = f"make -j{build['args']['parallel']}"
            elif build['args']['generator'] == 'Ninja':
                build_command = f"ninja -j{build['args']['parallel']}"

            build['cmake_args'] = [ '-G', build['args']['generator'] ] + build['cmake_args']

            if 'valgrind' in build['args'] and build['args']['valgrind'] is not None:
                build['cmake_args'].append('-DENABLE_VALGRIND=ON')

                if args['valgrind'] != True:
                    build['cmake_args'].append(f"-DVALGRIND_TOOLS={args['valgrind']}")
            else:
                build['cmake_args'].append('-DENABLE_VALGRIND=OFF')


            print()
            print(f"Combined build arguments for {build['name']}:")
            print(f"    {' '.join(build_command_line(build['args']))}")
            print()

            if build['args']['test']:
                build['cmake_args'].append('-DBUILD_TESTING=ON')

            print(f"CMake arguments for {build['name']}:")
            print(f"    {' '.join(build['cmake_args'])}")
            print()
            
            subprocess.run(
                [ cmake ] + build['cmake_args'] + [ build['args']['source'] ],
                cwd=build['args']['workspace'],
            )

            subprocess.run(
                [
                    shell, '-c',
                    build_command
                ],
                cwd=build['args']['workspace'],
            )

            if args['test']:
                # You can run ctest -j to run tests in parallel, but in order to capture the output in log files
                # we iterate over the tests manually.

                TEST_DATA_FILENAME = os.path.join(args['workspace'], 'mdsplus-test.json')

                result = subprocess.run(
                    [
                        ctest, '-N', '--show-only=json-v1'
                    ],
                    capture_output=True,
                    cwd=build['args']['workspace'],
                )

                if result.returncode != 0:
                    # TODO:
                    exit(1)

                test_queue = []
                test_data = json.loads(result.stdout.decode())

                prefix = ''
                if build['name'] != 'default':
                    prefix = f"{build['name']}/"
                
                for i, test in enumerate(test_data['tests']):
                    test_queue.append({
                        'index': i + 1,
                        'name': prefix + test['name'],
                        'build': build['name'],
                        'workspace': build['args']['workspace'],
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

                        log_filename = os.path.join(test['workspace'], 'testing', test['name'] + '.log')
                        os.makedirs(os.path.dirname(log_filename), exist_ok=True)
                        log_file = open(log_filename, 'wb')

                        print(f"[{test['index']:3}/{test_count}] Running: {test['name']}")

                        test_start_time = datetime.now()
                        
                        test_process = subprocess.Popen(
                            [ ctest, '-I', f"{test['index']},{test['index']}", '-V' ],
                            stdout=log_file,
                            stderr=subprocess.STDOUT,
                            cwd=test['workspace'],
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
                