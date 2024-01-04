# TODO: This only tests the "thin-client" API for MATLAB.  Eventually, should
# also incorporate the "thick-client" API tests in the "mdstest.m" file.
# NOTE: The following test harness and tests are derived from the IDL tests.

import os
import subprocess
import string

server = 'localhost'
if 'TEST_MDSIP_SERVER' in os.environ:
    server = os.environ['TEST_MDSIP_SERVER']

tree = os.environ['TEST_TREE']
shot = os.environ['TEST_SHOT']
node1 = os.environ['TEST_NODE1']
node1_value = os.environ['TEST_NODE1_VALUE']
node2 = os.environ['TEST_NODE2']
node2_value = os.environ['TEST_NODE2_VALUE']
dbname = os.environ['TEST_DB_NAME']
ascii = os.environ['TEST_ASCII']
ascii_value = os.environ['TEST_ASCII_VALUE']
numeric = os.environ['TEST_NUMERIC']
numeric_value = os.environ['TEST_NUMERIC_VALUE']
signal = os.environ['TEST_SIGNAL']
signal_value = os.environ['TEST_SIGNAL_VALUE']
fullpath = os.environ['TEST_FULLPATH']
fullpath_value = os.environ['TEST_FULLPATH_VALUE']
relative_def = os.environ['TEST_RELATIVE_DEF']
relative = os.environ['TEST_RELATIVE']
relative_value = os.environ['TEST_RELATIVE_VALUE']
reset_def = os.environ['TEST_RESET_DEF']
tag = os.environ['TEST_TAG']
tag_value = os.environ['TEST_TAG_VALUE']
wildcard = os.environ['TEST_WILDCARD']
wildcard_value = os.environ['TEST_WILDCARD_VALUE']
getnci = os.environ['TEST_GETNCI']
getnci_value = os.environ['TEST_GETNCI_VALUE']
rank = os.environ['TEST_RANK']
rank_value = os.environ['TEST_RANK_VALUE']
ndims = os.environ['TEST_NDIMS']
ndims_value = os.environ['TEST_NDIMS_VALUE']
dim_of = os.environ['TEST_DIM_OF']
dim_of_value = os.environ['TEST_DIM_OF_VALUE']
units_of = os.environ['TEST_UNITS_OF']
units_of_value = os.environ['TEST_UNITS_OF_VALUE']


#---------------------------------------------------------------------------
# Each write test should start with a clean tree.
# Write tests use a local tree, but eventually will be upgraded to use mdsip.
# TODO: The default_tree_path is temporary; will change when switch to mdsip.
#
def build_write_tree(tree, shot):
    import os
    import MDSplus as mds

    test_dir = os.getenv('MDSPLUS_DIR') + '/matlab/testing/'
    os.environ['default_tree_path'] = test_dir  # required to write tree

    tree_name = tree + '_' + str(shot)
    tree_file = test_dir + tree_name

    tc = tree_file + '.characteristics'
    td = tree_file + '.datafile'
    tt = tree_file + '.tree' 

    if os.path.exists(tc):
        os.remove(tc)
    if os.path.exists(td):
        os.remove(td)
    if os.path.exists(tt):
        os.remove(tt)

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

write_tree = 'write'
write_shot = 123
build_write_tree(write_tree, write_shot)


all_tests_passed = True
def matlab_test(code, expected_output):
    global all_tests_passed

    header_end = 'For product information, visit www.mathworks.com.'

    code_lines = [ line.strip() for line in code.splitlines() ]
    code_lines = list(filter(None, code_lines))
    code = '\n'.join(code_lines)

    code = '% placeholder comment\n' + code + '\nexit\n'
    open('test.m', 'wt').write(code)

    expected_lines = [ line.strip() for line in expected_output.splitlines() ]
    expected_lines = list(filter(None, expected_lines))

    print('Running:')
    for line in code_lines:
        line = line.replace(server, '******')
        line = line.replace(dbname, '******')
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

        if not hide_output:
            if line != '':
                print(line)
                lines.append(line.strip())
        
        if line == header_end:
            hide_output = False
    print()

    #stdout, _ = proc.communicate('test\n'.encode())
    #lines = [line.strip() for line in stdout.splitlines() ]
    lines = list(filter(None, lines))

    # Skip the MATLAB header and License information
    for i, line in enumerate(lines):
        if line == header_end:
            lines = lines[i + 1:]


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
mdsconnect('{server}');
mdsopen('{tree}', {shot});
disp(mdsvalue('{node1}'));
disp(mdsvalue('{node2}'));
mdsclose();
mdsdisconnect();

''',
f'''

{node1_value}
{node2_value}

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
                    
mdsconnect('{server}');
mdsopen('{tree}', {shot});
disp(mdsvalue('{ascii}'));
disp(mdsvalue('{numeric}'));
disp(transpose(mdsvalue('{signal}')));
disp(mdsvalue('{fullpath}'));
mdstcl('set def {relative_def}');
disp(mdsvalue('{relative}'));
mdstcl('set def {reset_def}');
disp(mdsvalue('{tag}'));
disp(mdsvalue('{wildcard}'));
disp(mdsvalue('{getnci}'));
disp(mdsvalue('{rank}'));
disp(mdsvalue('{ndims}'));
disp(transpose(mdsvalue('{dim_of}')));
disp(mdsvalue('{units_of}'));
mdsclose();
mdsdisconnect();

''',
f'''

{ascii_value}
{numeric_value}
{signal_value}
{fullpath_value}
{relative_value}
{tag_value}
{wildcard_value}
{getnci_value}
{rank_value}
{ndims_value}
{dim_of_value}
{units_of_value}

''')


#---------------------------------------------------------------------------
# Write: various permutations of writing data to a tree
#
# Note: Uses local tree on the build server (i.e., not using mdsip).
#       The $default_tree_path must point to the "matlab/testing" directory.
matlab_test(f'''
            
testid = 'MATLAB-write-various';         

mdsopen('{write_tree}', {write_shot});
mdsput('A_TEXT', ' "string_a" ');
mdsput('B_NUM', '22');
mdsput('C_SIGNAL', 'build_signal([10,-10,5,-5,0],[10.2,-10.2,5.4,-5.4,0.0], [0 .. 4])');
mdsput('\TOP.SUBTREE_1:D_TEXT', ' "string_d" ');
mdsput('SUBTREE_1.E_UNITS', 'build_with_units(55, "volts")');
mdstcl('set def SUBTREE_1');
mdsput('.-.SUBTREE_2:F_NUM', '66');
mdsput('\TAG_G', '77');
mdsclose();
         
mdsopen('{write_tree}', {write_shot});
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

if not all_tests_passed:
    exit(1)

