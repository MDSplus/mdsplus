#!/usr/bin/env python3

import sys

if not sys.version_info >= (3, 6):
    print('This script must be run with python >= 3.6')
    exit(1)

import os
import subprocess
import argparse
import signal
import shutil
import json

from datetime import datetime

deploy_dir = os.path.dirname(__file__)
root_dir = os.path.dirname(deploy_dir)

parser = argparse.ArgumentParser()

parser.add_argument(
    '--os',
    metavar='',
    help='',
)

parser.add_argument(
    '--test',
    action=argparse.BooleanOptionalAction,
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
)

parser.add_argument(
    '--sanitize',
    metavar='',
    help='',
)

args, unknown_args = parser.parse_known_args()
cmake_args = unknown_args

if args.os is not None:

    opts_filename = os.path.join(deploy_dir, f'os/{args.os}.opts')
    if not os.path.exists(opts_filename):
        print(f'Unsupported --os={args.os}, ensure that deploy/os/{args.os}.opts exists')
        exit(1)

    env_filename = os.path.join(deploy_dir, f'os/{args.os}.env')
    if os.path.exists(env_filename):
        args.env_file = env_filename
    
    opts = open(opts_filename).read().strip().split()
    args, unknown_args = parser.parse_known_args(opts, args)
    cmake_args.extend(unknown_args)

if args.source is None:
    args.source = root_dir

if args.workspace is None:
    if args.os is None:
        args.workspace = 'build/'
    else:
        branch = 'cmake' # TODO:
        args.workspace = f'build/{args.os}/{branch}'

# Convert the workspace to an absolute path
if not os.path.isabs(args.workspace):
    args.workspace = os.path.join(root_dir, args.workspace)

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

    docker_image_list = args.dockerimage.split(',')
    for docker_image in docker_image_list:

        if args.dockerpull:
            subprocess.run([ 'docker', 'pull', docker_image ])
        
        docker_args = [
            '--rm',
            '--volume', f'{root_dir}:{args.source}', # TODO: Improve?
            '--volume', f'{args.workspace}:/workspace',
            '--workdir', '/workspace',
            docker_image,
            '/bin/bash', '-c',
            'mkdir -p ~; git config --global --add safe.directory \*; ' # TODO: Fix
            f"python3 {args.source}/deploy/build.py {' '.join(passthrough_args)} {' '.join(cmake_args)}"
        ]

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
            [ shell, '-c', f'. {args.env_file}; env'],
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

    cmake_args.extend([ '-G', args.generator ])

    print()
    print('Combined build arguments:')
    print(f"    {' '.join(build_args)}")
    print()

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
            test_queue = [ test['name'] for test in test_data['tests'] ]

            test_index = 0
            test_count = len(test_queue)

            running_test_list = [ ]
            while len(test_queue) > 0 or len(running_test_list) > 0:

                for test in running_test_list:
                    result = test['process'].poll()
                    if result is not None:
                        running_test_list.remove(test)

                        delta_time = datetime.now() - test['start']
                        delta_time = delta_time.total_seconds()

                        result_message = 'Success' if result == 0 else 'Failed'
                        print(f"[{test['index']}/{test_count}] {result_message}: {test['name']} {delta_time:.3f}s")

                        if result != 0:
                            print(f"[{test['index']}/{test_count}] Log File: {test['log']}")

                while len(test_queue) > 0 and len(running_test_list) < int(args.parallel):
                    test_name = test_queue.pop(0)

                    log_filename = os.path.join(args.workspace, 'testing', test_name + '.log')
                    os.makedirs(os.path.dirname(log_filename), exist_ok=True)
                    log_file = open(log_filename, 'wb')

                    print(f'[{test_index}/{test_count}] Running: {test_name}')

                    start_time = datetime.now()
                    
                    test_process = subprocess.Popen(
                        [ ctest, '-R', f'^{test_name}$', '-V' ],
                        stdout=log_file,
                        stderr=subprocess.STDOUT,
                        cwd=args.workspace,
                    )
                    
                    running_test_list.append({
                        'index': test_index,
                        'start': start_time,
                        'name': test_name,
                        'process': test_process,
                        'log': log_filename,
                    })

                    test_index += 1
            