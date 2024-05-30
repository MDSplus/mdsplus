#!/usr/bin/env python3

import subprocess
import shutil

git_executable = shutil.which('git')

def git(command):
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

# Commits that start with 'Revert' have the same effect as their original counterparts

version_bump = 'SAME'
for commit in commit_log.splitlines():
    commit = commit.lower()

    # Commits that start with 'Release' will use the exact version that commit is tagged with
    if commit.startswith('release'):
        version_bump = 'SAME'
        break

    # Commits that start with 'Feature' will increment the minor version (0.x.0)
    elif commit.startswith('feature') or commit.startswith('revert "feature'):
        version_bump = 'MINOR'

    # Commits that start with 'Fix' will increment the patch version (0.0.x)
    elif commit.startswith('fix') or commit.startswith('revert "fix'):
        if version_bump != 'MINOR':
            version_bump = 'PATCH'
    
    # Commits that start with 'Tests' will not change the version
    elif commit.startswith('tests') or commit.startswith('revert "tests'):
        pass

    # Commits that start with 'Build' will not change the version
    elif commit.startswith('build') or commit.startswith('revert "build'):
        pass

    # Commits that start with 'Docs' will not change the version
    elif commit.startswith('docs') or commit.startswith('revert "docs'):
        pass

    # Commits without one of these prefixes will increment the patch version (0.0.x)
    else: 
        if version_bump != 'MINOR':
            version_bump = 'PATCH'

version_parts = last_release.split('-')
if len(version_parts) < 4:
    print('0.0.0')
    exit(1)
else:
    major = int(version_parts[-3])
    minor = int(version_parts[-2])
    patch = int(version_parts[-1])

if version_bump == 'MAJOR':
    major += 1
    minor = 0
    patch = 0
elif version_bump == 'MINOR':
    minor += 1
    patch = 0
elif version_bump == 'PATCH':
    patch += 1

print(f'{major}.{minor}.{patch}')