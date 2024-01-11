#!/usr/bin/env python3

import subprocess
import shutil

git_executable = shutil.which('git')

def git(command):
    # print(f'git {command}')
    proc = subprocess.Popen(
        [ git_executable ] + command.split(),
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT
    )

    stdout, _ = proc.communicate()
    return stdout.decode().strip()

last_release = git('describe --tags --abbrev=0')
last_release_commit = git(f'rev-list -n 1 {last_release}')
commit_log = git(f'log {last_release_commit}..HEAD --no-merges --decorate=short --pretty=format:%s')

version_bump = 'SAME'
for commit in commit_log.splitlines():
    commit = commit.lower()

    if commit.startswith('feature') or commit.startswith('revert "feature'):
        version_bump = 'MINOR'

    elif commit.startswith('fix') or commit.startswith('revert "fix'):
        if version_bump != 'MINOR':
            version_bump = 'PATCH'
    
    elif commit.startswith('tests') or commit.startswith('revert "tests'):
        pass

    elif commit.startswith('build') or commit.startswith('revert "build'):
        pass

    elif commit.startswith('docs') or commit.startswith('revert "docs'):
        pass

version_parts = last_release.split('-')
if len(version_parts) < 4:
    print('1.2.3')
    exit(1)
else:
    major = int(version_parts[-3])
    minor = int(version_parts[-2])
    patch = int(version_parts[-1])

if version_bump == 'MAJOR':
    major += 1
elif version_bump == 'MINOR':
    minor += 1
elif version_bump == 'PATCH':
    patch += 1

print(f'{major}.{minor}.{patch}')