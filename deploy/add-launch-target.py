#!/usr/bin/env python3

import os
import json
import argparse
import platform

parser = argparse.ArgumentParser()

parser.add_argument(
    '--name',
    required=True,
    help='Name of the config target to generate.'
)

parser.add_argument(
    '--command',
    required=True,
    help='Semicolon-delimted list describing how to run the program, e.g. program;arg1;arg2.'
)

parser.add_argument(
    '--environment',
    required=True,
    help='Semicolon-delimted list of environment modifications, see https://cmake.org/cmake/help/latest/prop_test/ENVIRONMENT_MODIFICATION.html.'
)

parser.add_argument(
    '--cwd',
    required=True,
    help='The working directory of the target.'
)

args = parser.parse_args()

target_name = args.name
command = args.command.split(';')
environment_modifications = args.environment.split(';')
cwd = args.cwd

def add_or_update_config(data, configurations):
    found = False
    for i in range(0, len(configurations)):
        if configurations[i]['name'] == data['name']:
            configurations[i].update(data)
            found = True
            break
    
    if not found:
        configurations.append(data)

###
### Visual Studio Code
###

if not os.path.isdir('.vscode'):
    os.mkdir('.vscode')

filename = '.vscode/launch.json'

config = {}
try:
    file = open(filename, 'r')
    config = json.load(file)
except:
    pass

if 'version' not in config:
    config['version'] = '0.2.0'

if 'configurations' not in config:
    config['configurations'] = []

env = {}

# https://cmake.org/cmake/help/latest/prop_test/ENVIRONMENT_MODIFICATION.html
for env_mod in environment_modifications:
    name, modification = env_mod.split('=', 1)
    action, value = modification.split(':', 1)

    if name not in env:
        env[name] = '${env:' + name + '}'

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

data = {
    'name': target_name,
    'type': 'cppdbg',
    'request': 'launch',
    'program': command[0],
    'args': command[1:],
    'cwd': cwd,
    'environment': [ { 'name': name, 'value': value } for name, value in env.items() ]
}

if platform.system() == 'Windows':
    data['type'] = 'cppvsdbg'
    data['console'] = 'integratedTerminal'
else:
    data['externalConsole'] = False

if platform.system() == 'Darwin':
    data['MIMode'] = 'lldb'

add_or_update_config(data, config['configurations'])

file = open(filename, 'w')
json.dump(config, file, indent=4)
