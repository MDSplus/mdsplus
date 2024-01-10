
import os
import argparse

parser = argparse.ArgumentParser()

parser.add_argument(
    '--output',
    required=True,
    help='Name of the environment file to write.'
)

parser.add_argument(
    '--environment',
    required=True,
    help='Semicolon-delimted list of environment modifications, see https://cmake.org/cmake/help/latest/prop_test/ENVIRONMENT_MODIFICATION.html.'
)

args = parser.parse_args()

output_filename = args.output
environment_modifications = args.environment.split(';')

env = {}

# https://cmake.org/cmake/help/latest/prop_test/ENVIRONMENT_MODIFICATION.html
for env_mod in environment_modifications:
    name, modification = env_mod.split('=', 1)
    action, value = modification.split(':', 1)

    if name not in env:
        # TODO: Windows
        env[name] = '${' + name + '}'

    if action == 'reset':
        del env[name]
    elif action == 'set':
        env[name] = value
    elif action == 'unset':
        env[name] = ''
    elif action == 'string_append':
        env[name] += value
    elif action == 'string_prepend':
        env[name] = value + env[name]
    elif action == 'path_list_append':
        env[name] += os.pathsep + value
    elif action == 'path_list_prepend':
        env[name] = value + os.pathsep + env[name]
    elif action == 'cmake_list_append':
        env[name] += ';' + value
    elif action == 'cmake_list_prepend':
        env[name] = value + ';' + env[name]

with open(output_filename, 'wt') as f:
    for key, value in env.items():
        f.write(f'{key}={value}\n')