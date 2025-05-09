#!/usr/bin/env python3

import argparse
import os
import sys

from pathlib import Path

import xml.etree.ElementTree as xml

parser = argparse.ArgumentParser()

parser.add_argument(
    '--junit-suite-name',
    metavar='',
    help='',
)

args = parser.parse_args()

# Using pathlib.Path.glob instead of glob.glob because it doesn't follow symlinks
# and the windows build directory has recursive symlinks
tap_filename_list = Path('.').glob('**/*.trs')

all_tests = {}
failed_test_count = 0

for tap_filename in tap_filename_list:

    name = os.path.relpath(os.path.splitext(tap_filename)[0], os.getcwd())

    log_filename = os.path.splitext(tap_filename)[0] + '.log'
    if not os.path.exists(log_filename):
        print(log_filename, 'not found')
        log_filename = None

    result = None
    
    lines = open(tap_filename, 'rt').readlines()
    for line in lines:
        line = line.strip()
        if line.startswith(':test-result: '):
            result = line.removeprefix(':test-result: ')
    
    if result == 'FAIL':
        failed_test_count += 1

    all_tests[name] = {
        'log': log_filename,
        'result': result,
    }

root = xml.Element('testsuites')
root.attrib['tests'] = str(len(all_tests))
root.attrib['failures'] = str(failed_test_count)

testsuite = xml.SubElement(root, 'testsuite')

# TODO: Improve
testsuite.attrib['name'] = 'mdsplus'
if args.junit_suite_name is not None:
    testsuite.attrib['name'] = args.junit_suite_name

for test_name, test in all_tests.items():
    testcase = xml.SubElement(testsuite, 'testcase')
    testcase.attrib['name'] = test_name

    system_out = xml.SubElement(testcase, 'system-out')
    system_out.text = open(test['log']).read()

    if test['result'] == 'FAIL':
        failure = xml.SubElement(testcase, 'failure')
        failure.attrib['message'] = 'Failed'

    if test['result'] == 'SKIP':
        failure = xml.SubElement(testcase, 'skipped')
        failure.attrib['message'] = 'Skipped'

print(f'Writing JUnit output to mdsplus-junit.xml')
with open('mdsplus-junit.xml', 'wb') as f:
    f.write(xml.tostring(root))