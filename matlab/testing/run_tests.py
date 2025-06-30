#!/usr/bin/env python3

# TODO: This only tests the "thin-client" API for MATLAB.  Eventually, should
# also incorporate the "thick-client" API tests in the "mdstest.m" file.
# NOTE: The following test harness and tests are derived from the IDL tests.

import os
import subprocess
import argparse
import string
import tempfile

# The default values are intended to be used from within the PSFC network
# If you want to run these tests on your own infrastructure, provide the
# details as arguments to customize the tests to your servers/trees/nodes

parser = argparse.ArgumentParser()

parser.add_argument(
    '--mdsip-server',
    default='alcdaq6',
    help='The server to call mdsconnect() on'
)

parser.add_argument(
    '--database-name',
    default='logbook',
    help='The database to call set_database() on'
)

parser.add_argument(
    '--tree',
    default='cmod',
    help='The tree to mdsopen()'
)

parser.add_argument(
    '--shot',
    default=1090909009,
    help='The shot to mdsopen()'
)

parser.add_argument(
    '--node1',
    default='SUM(\\IP)',
    help='An expression to evaluate and compare against --node1-value'
)

parser.add_argument(
    '--node1-value',
    default='-69662768',
    help='The value of evaluating the --node1 expression, ignoring leading/trailing whitespace'
)

parser.add_argument(
    '--node2',
    default='TSTART',
    help='An expression to evaluate and compare against --node2-value'
)

parser.add_argument(
    '--node2-value',
    default='-4',
    help='The value of evaluating the --node2 expression, ignoring leading/trailing whitespace'
)

parser.add_argument(
    '--text',
    default='ADMIN.LOGBOOK.STATISTICS:ANAL_DONE',
    help='An expression to evaluate and compare against --text-value'
)

parser.add_argument(
    '--text-value',
    default='9-SEP-2009 11:29:41.00',
    help='The value of evaluating the --text expression, ignoring leading/trailing whitespace'
)

parser.add_argument(
    '--numeric',
    default='SEQUENCE_NUM',
    help='An expression to evaluate and compare against --numeric-value'
)

parser.add_argument(
    '--numeric-value',
    default='35575',
    help='The value of evaluating the --numeric expression, ignoring leading/trailing whitespace'
)

parser.add_argument(
    '--signal',
    default='ADMIN.FAST_WINDOW.XTOMO:OPTIONS',
    help='An expression to evaluate and compare against --signal-value'
)

parser.add_argument(
    '--signal-value',
    default="{'none   '}    {'25kHz  '}    {'50kHz  '}    {'83.3kHz'}",
    help='The value of evaluating the --signal expression, ignoring leading/trailing whitespace'
)

parser.add_argument(
    '--fullpath',
    default='\\CMOD::TOP.ELECTRONS.ECE.RESULTS:APERTURE',
    help='An expression to evaluate and compare against --fullpath-value'
)

parser.add_argument(
    '--fullpath-value',
    default='39.1000',
    help='The value of evaluating the --fullpath expression, ignoring leading/trailing whitespace'
)

parser.add_argument(
    '--relative-def',
    default='\\CMOD::TOP.ELECTRONS.ECE',
    help='An expression to change the default position in the tree'
)

parser.add_argument(
    '--relative',
    default='.RESULTS:ZPD',
    help='An expression to evaluate and compare against --relative-value'
)

parser.add_argument(
    '--relative-value',
    default='57',
    help='The value of evaluating the --relative expression, ignoring leading/trailing whitespace'
)

parser.add_argument(
    '--reset-def',
    default='\\CMOD::TOP',
    help='An expression to reset the default position to the top of the tree'
)

parser.add_argument(
    '--tag',
    default='\\DNB::TSTART',
    help='An expression to evaluate and compare against --tag-value'
)

parser.add_argument(
    '--tag-value',
    default='-4',
    help='The value of evaluating the --tag expression, ignoring leading/trailing whitespace'
)

# The parser is converting "\\" into "\", so to get desired path of "\\CMOD::" must use "\\\\"
parser.add_argument(
    '--wildcard',
    default='SUBSCRIPT(GETNCI("\\\\CMOD::TOP.***:CPLD_START", "FULLPATH"),0)',
    help='An expression to evaluate and compare against --wildcard-value'
)

parser.add_argument(
    '--wildcard-value',
    default='\\CMOD::TOP.DNB.MIT_CXRS:CPLD_START',
    help='The value of evaluating the --wildcard expression, ignoring leading/trailing whitespace'
)

# The parser is converting "\\" into "\", so to get desired path of "\\CMOD::" must use "\\\\"
parser.add_argument(
    '--getnci',
    default=' SUBSCRIPT(GETNCI("\\\\CMOD::TOP.DNB.MIT_CXRS:CPLD_START", "USAGE"),0) ',
    help='An expression to evaluate and compare against --getnci-value'
)

parser.add_argument(
    '--getnci-value',
    default='5',
    help='The value of evaluating the --getnci expression, ignoring leading/trailing whitespace'
)

parser.add_argument(
    '--rank',
    default='RANK(\\IP)',
    help='An expression to evaluate and compare against --rank-value'
)

parser.add_argument(
    '--rank-value',
    default='1',
    help='The value of evaluating the --rank expression, ignoring leading/trailing whitespace'
)

parser.add_argument(
    '--ndims',
    default='NDIMS(\\IP)',
    help='An expression to evaluate and compare against --ndims-value'
)

parser.add_argument(
    '--ndims-value',
    default='1',
    help='The value of evaluating the --ndims expression, ignoring leading/trailing whitespace'
)

parser.add_argument(
    '--dim-of',
    default='DIM_OF(\\CMOD::TOP.ADMIN.FAST_WINDOW.XTOMO:OPTIONS, 0)',
    help='An expression to evaluate and compare against --dim-of-value'
)

parser.add_argument(
    '--dim-of-value',
    default='1   1   1   1',
    help='The value of evaluating the --dim-of expression, ignoring leading/trailing whitespace'
)

parser.add_argument(
    '--units-of',
    default='UNITS_OF(\\CMOD::TOP.MHD.XTOMO.SIGNALS.ARRAY_1:CHORD_01)',
    help='An expression to evaluate and compare against --units-of-value'
)

parser.add_argument(
    '--units-of-value',
    default='watts',
    help='The value of evaluating the --units-of expression, ignoring leading/trailing whitespace'
)

parser.add_argument(
    '--write-tree',
    default='matlab_tests',
    help='Name of tree to create for exercising write functions of the API'
)

parser.add_argument(
    '--write-shot',
    default=100,
    help='Shot number of tree to create for exercising write functions of the API'
)

args = parser.parse_args()


#---------------------------------------------------------------------------
# Each write test should start with a clean tree.
# Write tests use a local tree, but eventually will be upgraded to use mdsip.
#
def build_write_tree(tree, shot):
    import MDSplus as mds

    t = mds.Tree(tree, shot, 'new')

    nid = t.addNode('A_TEXT', 'text')
    nid = t.addNode('B_NUM', 'numeric')
    nid = t.addNode('C_SIGNAL', 'signal')

    nid = t.addNode('SUBTREE_1', 'structure')
    nid = t.addNode('SUBTREE_1.D_TEXT', 'text')
    nid = t.addNode('SUBTREE_1.E_UNITS', 'numeric')

    nid = t.addNode('SUBTREE_2', 'structure')
    nid = t.addNode('SUBTREE_2.F_NUM', 'numeric')
    nid = t.addNode('SUBTREE_2.G_NUM', 'numeric')
    nid.addTag('TAG_G')

    t.write()
    t.close()


# Temporary directory for transient test scripts and artifacts
TEST_DIR = tempfile.TemporaryDirectory(prefix='test_matlab_', dir='/tmp')
os.environ['default_tree_path'] = TEST_DIR.name
os.environ['MATLABPATH'] = os.getenv('MATLABPATH') + ':' + TEST_DIR.name

build_write_tree(args.write_tree, args.write_shot)

all_tests_passed = True
def matlab_test(code, expected_output):
    global all_tests_passed

    header_end = 'For product information, visit www.mathworks.com.'

    code_lines = [ line.strip() for line in code.splitlines() ]
    code_lines = list(filter(None, code_lines))
    code = '\n'.join(code_lines)

    # Write the test to a MATLAB script file, test.m

    code = '% placeholder comment\n' + code + '\nexit\n'
    test_file = TEST_DIR.name + '/test.m'
    open(test_file, 'wt').write(code)

    expected_lines = [ line.strip() for line in expected_output.splitlines() ]
    expected_lines = list(filter(None, expected_lines))

    print('Running:')
    for line in code_lines:
        print(f'MATLAB> {line}')
    print()

    print('Expected:')
    for line in expected_lines:
        print(line)
    print()

    proc = subprocess.Popen(
        ['matlab', '-nodisplay'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT
    )

    # We call our test.m module

    proc.stdin.write('test\n'.encode())
    proc.stdin.flush()
    
    hide_output = True
    strip_set = string.whitespace + ">"  # Excludes MATLAB's >> prompt

    print('Actual:')
    lines = []
    while True:
        line = proc.stdout.readline().decode()
        if not line:
            break

        line = line.strip(strip_set)

        if line is not None and not hide_output:
            if line != '':
                print(line)
                lines.append(line.strip())
        
        # Skip the MATLAB header and licensing information, which is everything
        # above this line
        if line == header_end:
            hide_output = False
    print()

    this_test_passed = True
 
    for line, expected in zip(lines, expected_lines):
        if line != expected:
            print(f'`{line}` != `{expected}`')
            this_test_passed = False
            all_tests_passed = False

    if this_test_passed:
        print('Success')
    else:
        print('Failure')

    print()
    print('----------------------------------------------------------------\n')
    print()


#-------------------------------------------------------------------------------
# Tree open / read / close
matlab_test(f'''
         
testid = 'MATLAB-tree-read';
mdsconnect('{args.mdsip_server}');
mdsopen('{args.tree}', {args.shot});
disp(mdsvalue('{args.node1}'));
disp(mdsvalue('{args.node2}'));
mdsclose();
mdsdisconnect();

''',
f'''

{args.node1_value}
{args.node2_value}

''')


#-------------------------------------------------------------------------------
# https://github.com/MDSplus/mdsplus/issues/2639
# Issue #2639: mdsvalue works without a socket
matlab_test(f'''
         
testid = 'MATLAB-2639-no-socket';
DATA = '55';
disp(mdsvalue(DATA));

''',
'''

55

''')


#---------------------------------------------------------------------------
# Read: various permutations of reading data from a tree
matlab_test(f'''
            
testid = 'MATLAB-read-various';
                    
mdsconnect('{args.mdsip_server}');
mdsopen('{args.tree}', {args.shot});
disp(mdsvalue('{args.text}'));
disp(mdsvalue('{args.numeric}'));
disp(transpose(mdsvalue('{args.signal}')));
disp(mdsvalue('{args.fullpath}'));
mdstcl('set def {args.relative_def}');
disp(mdsvalue('{args.relative}'));
mdstcl('set def {args.reset_def}');
disp(mdsvalue('{args.tag}'));
disp(mdsvalue('{args.wildcard}'));
disp(mdsvalue('{args.getnci}'));
disp(mdsvalue('{args.rank}'));
disp(mdsvalue('{args.ndims}'));
disp(transpose(mdsvalue('{args.dim_of}')));
disp(mdsvalue('{args.units_of}'));
mdsclose();
mdsdisconnect();

''',
f'''

{args.text_value}
{args.numeric_value}
{args.signal_value}
{args.fullpath_value}
{args.relative_value}
{args.tag_value}
{args.wildcard_value}
{args.getnci_value}
{args.rank_value}
{args.ndims_value}
{args.dim_of_value}
{args.units_of_value}

''')


#---------------------------------------------------------------------------
# Write: various permutations of writing data to a tree
#
# Note: Uses local tree on the build server (i.e., not using mdsip).
#       The $default_tree_path must point to the "matlab/testing" directory.
matlab_test(f'''
            
testid = 'MATLAB-write-various';

mdsopen('{args.write_tree}', {args.write_shot});
mdsput('A_TEXT', ' "string_a" ');
mdsput('B_NUM', '22');
mdsput('C_SIGNAL', 'build_signal([10,-10,5,-5,0],[10.2,-10.2,5.4,-5.4,0.0], [0 .. 4])');
mdsput('\\TOP.SUBTREE_1:D_TEXT', ' "string_d" ');
mdsput('SUBTREE_1.E_UNITS', 'build_with_units(55, "volts")');
mdstcl('set def SUBTREE_1');
mdsput('.-.SUBTREE_2:F_NUM', '66');
mdsput('\\TAG_G', '77');
mdsclose();
         
mdsopen('{args.write_tree}', {args.write_shot});
disp(mdsvalue('A_TEXT'));
disp(mdsvalue('B_NUM'));
disp(transpose(mdsvalue('DATA(C_SIGNAL)')));
disp(transpose(mdsvalue('RAW_OF(C_SIGNAL)')));
disp(transpose(mdsvalue('DIM_OF(C_SIGNAL)')));
disp(mdsvalue('SUBTREE_1:D_TEXT'));
disp(mdsvalue('SUBTREE_1.E_UNITS'));
disp(mdsvalue('UNITS_OF(SUBTREE_1.E_UNITS)'));
disp(mdsvalue('SUBTREE_2:F_NUM'));
disp(mdsvalue('SUBTREE_2:G_NUM'));
mdsclose();

''',
'''

string_a
22
10   -10     5    -5     0
10.2000  -10.2000    5.4000   -5.4000         0
0   1   2   3   4
string_d
55
volts
66
77

''')


# ---------------------------------------------------------------------
# test type conversion from/to MATLAB
matlab_test(f'''

addpath('../');
run("run_tests.m");

''',
'''

PASSED:     115
FAILED:       0
INCOMPLETE:   0

''')

if not all_tests_passed:
    exit(1)

