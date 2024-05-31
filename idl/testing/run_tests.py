#!/usr/bin/env python3

import os
import subprocess
import argparse
import tempfile
import threading

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
    default='-6.96628e+07',
    help='The value of evaluating the --node1 expression, ignoring leading/trailing whitespace'
)

parser.add_argument(
    '--node2',
    default='TSTART',
    help='An expression to evaluate and compare against --node2-value'
)

parser.add_argument(
    '--node2-value',
    default='-4.00000',
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
    default='none 25kHz 50kHz 83.3kHz',
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
    default='\CMOD::TOP.ELECTRONS.ECE',
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
    default='\CMOD::TOP',
    help='An expression to reset the default position to the top of the tree'
)

parser.add_argument(
    '--tag',
    default='\\DNB::TSTART',
    help='An expression to evaluate and compare against --tag-value'
)

parser.add_argument(
    '--tag-value',
    default='-4.00000',
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
    default='\CMOD::TOP.DNB.MIT_CXRS:CPLD_START',
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
    default='1           1           1           1',
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
    default='idl_tests',
    help='Name of tree to create for exercising write functions of the API'
)

parser.add_argument(
    '--write-shot',
    default=100,
    help='Shot number of tree to create for exercising write functions of the API'
)

args = parser.parse_args()

#---------------------------------------------------------------------------
# Each IDL write test should start with a clean tree.
# Write tests use a local tree, but eventually will be upgraded to use mdsip.
# TODO: The default_tree_path is temporary; will change when switch to mdsip.
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


class IDLTest(threading.Thread):

    def __init__(self, name, code, expected_output):
        super(IDLTest, self).__init__(name=name)

        self.name = name
        self.code = code
        self.expected_output = expected_output
        self.passed = False

        self.logfile = os.path.join(os.getcwd(), f'{self.name}.log')

    def run(self):

        log = open(self.logfile, 'wt')

        # Temporary directory for transient test scripts and artifacts
        TEST_DIR = tempfile.TemporaryDirectory(prefix='test_idl_', dir='/tmp')

        code_lines = [ line.strip() for line in self.code.splitlines() ]
        code_lines = list(filter(None, code_lines))
        code = '\n'.join(code_lines)

        # Running IDL code at the IDL> prompt vs running it as a script causes
        # weird differences in the evaluation, so we use a test.pro file

        code = 'pro test\n' + code + '\nend'
        test_file = TEST_DIR.name + '/test.pro'
        open(test_file, 'wt').write(code)

        expected_lines = [ line.strip() for line in self.expected_output.splitlines() ]
        expected_lines = list(filter(None, expected_lines))

        log.write(f'Running {self.name}:\n')
        for line in code_lines:
            log.write(f'IDL> {line}\n')
        log.write('\n')

        log.write('Expected:\n')
        for line in expected_lines:
            log.write(f'{line}\n')
        log.write('\n')

        env = dict(os.environ)
        env['IDL_PATH'] = env['IDL_PATH'] + ':' + TEST_DIR.name

        proc = subprocess.Popen(
            ['idl'],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            env=env
        )

        # We call our test.pro module

        proc.stdin.write('test\nexit\n'.encode())
        proc.stdin.flush()

        hide_output = True

        log.write('Actual:\n')
        lines = []
        while True:
            line = proc.stdout.readline().decode()
            if not line:
                break

            line = line.strip()

            if line is not None and not hide_output:
                log.write(f'{line}\n')
                log.flush()
                if line != '':
                    lines.append(line.strip())

            # Skip the IDL header and licenseing information, which is everything
            # above this line
            if line == '% Compiled module: TEST.':
                hide_output = False
        log.write('\n')

        self.passed = True

        for line, expected in zip(lines, expected_lines):
            if line != expected:
                log.write(f'`{line}` != `{expected}`\n')
                self.passed = False

        if self.passed:
            log.write('Success\n')
        else:
            log.write(f'Failure, see {self.logfile}\n')

        log.write('\n')
        log.write('----------------------------------------------------------------\n')
        log.write('\n')

        log.close()

        with open(self.logfile, 'rt') as log:
            print(log.read(), flush=True)


all_tests = []
def idl_test(name, code, expected_output):
    global all_tests

    print(f'Starting {name}')

    t = IDLTest(name, code, expected_output)
    t.start()
    all_tests.append(t)


#-------------------------------------------------------------------------------
# Tree open / read / close
idl_test('IDL-tree-read',
f'''

mdsconnect, '{args.mdsip_server}'
mdsopen, '{args.tree}', {args.shot}
print, mdsvalue('{args.node1}')
print, mdsvalue('{args.node2}')
mdsclose, '{args.tree}', '{args.shot}'

''',
f'''

% Compiled module: MDSCONNECT.
% Compiled module: MDS_KEYWORD_SET.
% Compiled module: MDSDISCONNECT.
% Compiled module: MDSOPEN.
% Compiled module: MDSVALUE.
% Compiled module: MDSCHECKARG.
% Compiled module: MDSISCLIENT.
{args.node1_value}
{args.node2_value}
% Compiled module: MDSCLOSE.

''')


#-------------------------------------------------------------------------------
# https://github.com/MDSplus/mdsplus/issues/2580
# Issue 2580: first connect should be on socket 0.
# Very rare for returned status to be zero.
idl_test('IDL-2580-connect',
f'''

PASS = 1
FAIL = 0
BOGUS = -77
mdsip_server = '{args.mdsip_server}'
test_status = PASS

mdsconnect, mdsip_server
print, '!MDS_SOCKET=', !MDS_SOCKET
if (!MDS_SOCKET ne 0) then test_status = FAIL

val = BOGUS
mdsconnect, mdsip_server, status=val
print, '!MDS_SOCKET=', !MDS_SOCKET
print, 'status=', val
if (!MDS_SOCKET ne 1) then test_status = FAIL
if (val ne 1) then test_status = FAIL

if (test_status eq PASS) then begin
    print, 'SUCCESS'
end else begin
    print, 'FAILURE'
endelse


''',
'''

% Compiled module: MDSCONNECT.
% Compiled module: MDS_KEYWORD_SET.
% Compiled module: MDSDISCONNECT.
!MDS_SOCKET=       0
!MDS_SOCKET=       1
status=       1
SUCCESS

''')

if args.database_name != '':


    #---------------------------------------------------------------------------
    # https://github.com/MDSplus/mdsplus/issues/2625
    # Issue #2625: database on socket 0, subsequent connect doesn't break proxy.
    # If the queries work, the "val" variables will be changed to a text timestamp.
    idl_test('IDL-2625-simple',
    f'''

    PASS = 1
    FAIL = 0
    BOGUS = -77
    proxy = '{args.database_name}'
    mdsip_server = '{args.mdsip_server}'
    query = 'select getdate()'
    test_status = PASS

    set_database, proxy
    print, '!MDSDB_SOCKET=', !MDSDB_SOCKET
    val1 = BOGUS
    d = dsql(query, val1)
    if (typename(val1) ne "STRING") then test_status = FAIL

    mdsconnect, mdsip_server
    print, '!MDS_SOCKET=', !MDS_SOCKET
    val2 = BOGUS
    d = dsql(query, val2)
    if (typename(val2) ne "STRING") then test_status = FAIL

    if (test_status eq PASS) then begin
        print, 'SUCCESS'
    end else begin
        print, 'FAILURE'
    endelse

    ''',
    '''

    % Compiled module: SET_DATABASE.
    % Compiled module: MDS_KEYWORD_SET.
    % Compiled module: MDSVALUE.
    % Compiled module: MDSCHECKARG.
    % Compiled module: MDSISCLIENT.
    % Compiled module: DSQL.
    !MDSDB_SOCKET=       0
    % Compiled module: EVALUATE.
    % Compiled module: MDSDISCONNECT.
    !MDS_SOCKET=       1
    SUCCESS

    ''')


    #---------------------------------------------------------------------------
    # https://github.com/MDSplus/mdsplus/issues/2625
    # Issue #2625: usual pattern at GA is connect, then database.
    idl_test('IDL-2625-usual',
    f'''

    PASS = 1
    FAIL = 0
    BOGUS = -77
    proxy = '{args.database_name}'
    mdsip_server = '{args.mdsip_server}'
    query = 'select getdate()'
    test_status = PASS

    mdsconnect, mdsip_server
    print, '!MDS_SOCKET=', !MDS_SOCKET

    set_database, proxy
    print, '!MDSDB_SOCKET=', !MDSDB_SOCKET
    val1 = BOGUS
    d = dsql(query, val1)
    if (typename(val1) ne "STRING") then test_status = FAIL

    if (test_status eq PASS) then begin
        print, 'SUCCESS'
    end else begin
        print, 'FAILURE'
    endelse

    ''',
    '''

    % Compiled module: MDSCONNECT.
    % Compiled module: MDS_KEYWORD_SET.
    % Compiled module: MDSDISCONNECT.
    !MDS_SOCKET=       0
    % Compiled module: SET_DATABASE.
    % Compiled module: MDSVALUE.
    % Compiled module: MDSCHECKARG.
    % Compiled module: MDSISCLIENT.
    % Compiled module: DSQL.
    !MDSDB_SOCKET=       1
    % Compiled module: EVALUATE.
    SUCCESS

    ''')


    #---------------------------------------------------------------------------
    # https://github.com/MDSplus/mdsplus/issues/2625
    # Issue #2625: second database call not affected.
    idl_test('IDL-2625-two-database',
    f'''

    PASS = 1
    FAIL = 0
    BOGUS = -77
    proxy = '{args.database_name}'
    mdsip_server = '{args.mdsip_server}'
    query = 'select getdate()'
    test_status = PASS

    set_database, proxy
    print, '!MDSDB_SOCKET=', !MDSDB_SOCKET
    val1 = BOGUS
    d = dsql(query, val1)
    if (typename(val1) ne "STRING") then test_status = FAIL

    mdsconnect, mdsip_server
    print, '!MDS_SOCKET=', !MDS_SOCKET

    set_database, proxy
    print, '!MDSDB_SOCKET=', !MDSDB_SOCKET
    val2 = BOGUS
    d = dsql(query, val2)
    if (typename(val2) ne "STRING") then test_status = FAIL

    if (test_status eq PASS) then begin
        print, 'SUCCESS'
    end else begin
        print, 'FAILURE'
    endelse

    ''',
    '''

    % Compiled module: SET_DATABASE.
    % Compiled module: MDS_KEYWORD_SET.
    % Compiled module: MDSVALUE.
    % Compiled module: MDSCHECKARG.
    % Compiled module: MDSISCLIENT.
    % Compiled module: DSQL.
    !MDSDB_SOCKET=       0
    % Compiled module: EVALUATE.
    % Compiled module: MDSDISCONNECT.
    !MDS_SOCKET=       1
    !MDSDB_SOCKET=       2
    SUCCESS

    ''')


    #---------------------------------------------------------------------------
    # https://github.com/MDSplus/mdsplus/issues/2625
    # Issue #2625: stress test database proxy with many connects.
    # NLOOPS should be more than 64 (see Issue #2638).  If the
    # disconnect works, will never exceed the concurrent limit.
    idl_test('IDL-2625-loop',
    f'''

    PASS = 1
    FAIL = 0
    BOGUS = -77
    NLOOPS = 100
    proxy = '{args.database_name}'
    mdsip_server = '{args.mdsip_server}'
    query = 'select getdate()'
    test_status = PASS

    set_database, proxy
    print, '!MDSDB_SOCKET=', !MDSDB_SOCKET
    val1 = BOGUS
    d = dsql(query, val1)
    if (typename(val1) ne "STRING") then test_status = FAIL

    for i = 1, NLOOPS do begin
        socketvar=-1
        mdsconnect, mdsip_server, socket=socketvar
        print, 'socket=', socketvar
        val2 = BOGUS
        d = dsql(query, val2)
        if (typename(val2) ne "STRING") then test_status = FAIL
        mdsdisconnect, socket=socketvar
    endfor

    if (test_status eq PASS) then begin
        print, 'SUCCESS'
    end else begin
        print, 'FAILURE'
    endelse

    ''',
    '''

    % Compiled module: SET_DATABASE.
    % Compiled module: MDS_KEYWORD_SET.
    % Compiled module: MDSVALUE.
    % Compiled module: MDSCHECKARG.
    % Compiled module: MDSISCLIENT.
    % Compiled module: DSQL.
    !MDSDB_SOCKET=       0
    % Compiled module: EVALUATE.
    socket=           1
    % Compiled module: MDSDISCONNECT.
    socket=           2
    socket=           3
    socket=           4
    socket=           5
    socket=           6
    socket=           7
    socket=           8
    socket=           9
    socket=          10
    socket=          11
    socket=          12
    socket=          13
    socket=          14
    socket=          15
    socket=          16
    socket=          17
    socket=          18
    socket=          19
    socket=          20
    socket=          21
    socket=          22
    socket=          23
    socket=          24
    socket=          25
    socket=          26
    socket=          27
    socket=          28
    socket=          29
    socket=          30
    socket=          31
    socket=          32
    socket=          33
    socket=          34
    socket=          35
    socket=          36
    socket=          37
    socket=          38
    socket=          39
    socket=          40
    socket=          41
    socket=          42
    socket=          43
    socket=          44
    socket=          45
    socket=          46
    socket=          47
    socket=          48
    socket=          49
    socket=          50
    socket=          51
    socket=          52
    socket=          53
    socket=          54
    socket=          55
    socket=          56
    socket=          57
    socket=          58
    socket=          59
    socket=          60
    socket=          61
    socket=          62
    socket=          63
    socket=          64
    socket=          65
    socket=          66
    socket=          67
    socket=          68
    socket=          69
    socket=          70
    socket=          71
    socket=          72
    socket=          73
    socket=          74
    socket=          75
    socket=          76
    socket=          77
    socket=          78
    socket=          79
    socket=          80
    socket=          81
    socket=          82
    socket=          83
    socket=          84
    socket=          85
    socket=          86
    socket=          87
    socket=          88
    socket=          89
    socket=          90
    socket=          91
    socket=          92
    socket=          93
    socket=          94
    socket=          95
    socket=          96
    socket=          97
    socket=          98
    socket=          99
    socket=         100
    SUCCESS

    ''')


#-------------------------------------------------------------------------------
# https://github.com/MDSplus/mdsplus/issues/2638
# Issue #2638: crashes with too many concurrent sockets.
# NLOOPS should be more than 64.
idl_test('IDL-2638-loop',
f'''

PASS = 1
FAIL = 0
NLOOPS = 100
mdsip_server = '{args.mdsip_server}'
test_status = FAIL

for i = 1, NLOOPS do begin
    socketvar=-1
    mdsconnect, mdsip_server, socket=socketvar
    print, 'socket=', socketvar
endfor
test_status = PASS

if (test_status eq PASS) then begin
    print, 'SUCCESS'
end else begin
    print, 'FAILURE'
endelse

''',
'''

% Compiled module: MDSCONNECT.
% Compiled module: MDS_KEYWORD_SET.
socket=           0
socket=           1
socket=           2
socket=           3
socket=           4
socket=           5
socket=           6
socket=           7
socket=           8
socket=           9
socket=          10
socket=          11
socket=          12
socket=          13
socket=          14
socket=          15
socket=          16
socket=          17
socket=          18
socket=          19
socket=          20
socket=          21
socket=          22
socket=          23
socket=          24
socket=          25
socket=          26
socket=          27
socket=          28
socket=          29
socket=          30
socket=          31
socket=          32
socket=          33
socket=          34
socket=          35
socket=          36
socket=          37
socket=          38
socket=          39
socket=          40
socket=          41
socket=          42
socket=          43
socket=          44
socket=          45
socket=          46
socket=          47
socket=          48
socket=          49
socket=          50
socket=          51
socket=          52
socket=          53
socket=          54
socket=          55
socket=          56
socket=          57
socket=          58
socket=          59
socket=          60
socket=          61
socket=          62
socket=          63
socket=          64
socket=          65
socket=          66
socket=          67
socket=          68
socket=          69
socket=          70
socket=          71
socket=          72
socket=          73
socket=          74
socket=          75
socket=          76
socket=          77
socket=          78
socket=          79
socket=          80
socket=          81
socket=          82
socket=          83
socket=          84
socket=          85
socket=          86
socket=          87
socket=          88
socket=          89
socket=          90
socket=          91
socket=          92
socket=          93
socket=          94
socket=          95
socket=          96
socket=          97
socket=          98
socket=          99
SUCCESS

''')


#-------------------------------------------------------------------------------
# https://github.com/MDSplus/mdsplus/issues/2639
# Issue #2639: mdsvalue works without a socket
idl_test('IDL-2639-no-socket',
f'''

PASS = 1
FAIL = 0
DATA = '55'
mdsip_server = '{args.mdsip_server}'
test_status = PASS

result = mdsvalue(DATA)
print, 'result=', result
if (result ne fix(DATA)) then test_status = FAIL

if (test_status eq PASS) then begin
    print, 'SUCCESS'
end else begin
    print, 'FAILURE'
endelse

''',
'''

% Compiled module: MDSVALUE.
% Compiled module: MDSCHECKARG.
% Compiled module: MDSISCLIENT.
% Compiled module: MDS_KEYWORD_SET.
% Compiled module: MDSIDLIMAGE.
result=          55
SUCCESS

''')


#-------------------------------------------------------------------------------
# https://github.com/MDSplus/mdsplus/issues/2639
# Issue #2639: mdsvalue and interaction with killed socket.
# idl_test('IDL-2639-kill-last-socket',
# f'''

# PASS = 1
# FAIL = 0
# DATA = '55'
# mdsip_server = '{args.mdsip_server}'
# test_status = PASS

# mdsconnect, mdsip_server
# mdsconnect, mdsip_server

# result = mdsvalue(DATA)
# print, 'result=', result
# if (result ne fix(DATA)) then test_status = FAIL

# mdsdisconnect, socket=1

# result = mdsvalue(DATA)
# print, 'result=', result
# if (result ne fix(DATA)) then test_status = FAIL

# if (test_status eq PASS) then begin
#     print, 'SUCCESS'
# end else begin
#     print, 'FAILURE'
# endelse

# ''',
# '''

# % Compiled module: MDSCONNECT.
# % Compiled module: MDS_KEYWORD_SET.
# % Compiled module: MDSDISCONNECT.
# % Compiled module: MDSVALUE.
# % Compiled module: MDSCHECKARG.
# % Compiled module: MDSISCLIENT.
# result=          55
# result=          55
# SUCCESS

# ''')


#-------------------------------------------------------------------------------
# https://github.com/MDSplus/mdsplus/issues/2639
# Issue #2639: mdsvalue and interaction with killed socket 0.
idl_test('IDL-2639-kill-first-socket',
f'''

PASS = 1
FAIL = 0
DATA = '55'
mdsip_server = '{args.mdsip_server}'
test_status = PASS

mdsconnect, mdsip_server
mdsconnect, mdsip_server

result = mdsvalue(DATA)
print, 'result=', result
if (result ne fix(DATA)) then test_status = FAIL

mdsdisconnect, socket=0

result = mdsvalue(DATA)
print, 'result=', result
if (result ne fix(DATA)) then test_status = FAIL

if (test_status eq PASS) then begin
    print, 'SUCCESS'
end else begin
    print, 'FAILURE'
endelse

''',
'''

% Compiled module: MDSCONNECT.
% Compiled module: MDS_KEYWORD_SET.
% Compiled module: MDSDISCONNECT.
% Compiled module: MDSVALUE.
% Compiled module: MDSCHECKARG.
% Compiled module: MDSISCLIENT.
result=          55
result=          55
SUCCESS

''')


#-------------------------------------------------------------------------------
# https://github.com/MDSplus/mdsplus/issues/2639
# Issue #2639: mdsvalue and kill default socket.
idl_test('IDL-2639-kill-default-socket',
f'''

PASS = 1
FAIL = 0
DATA = '55'
mdsip_server = '{args.mdsip_server}'
test_status = PASS

mdsconnect, mdsip_server
mdsconnect, mdsip_server

result = mdsvalue(DATA)
print, 'result=', result
if (result ne fix(DATA)) then test_status = FAIL

mdsdisconnect

result = mdsvalue(DATA)
print, 'result=', result
if (result ne fix(DATA)) then test_status = FAIL

if (test_status eq PASS) then begin
    print, 'SUCCESS'
end else begin
    print, 'FAILURE'
endelse

''',
'''

% Compiled module: MDSCONNECT.
% Compiled module: MDS_KEYWORD_SET.
% Compiled module: MDSDISCONNECT.
% Compiled module: MDSVALUE.
% Compiled module: MDSCHECKARG.
% Compiled module: MDSISCLIENT.
result=          55
% Compiled module: MDSIDLIMAGE.
result=          55
SUCCESS

''')


#-------------------------------------------------------------------------------
# https://github.com/MDSplus/mdsplus/issues/2639
# Issue #2639: mdsvalue, one connect, and kill socket.
idl_test('IDL-2639-kill-single-socket',
f'''

PASS = 1
FAIL = 0
DATA = '55'
mdsip_server = '{args.mdsip_server}'
test_status = PASS

result = mdsvalue(DATA)
print, 'result=', result
if (result ne fix(DATA)) then test_status = FAIL

mdsconnect, mdsip_server
mdsdisconnect

result = mdsvalue(DATA)
print, 'result=', result
if (result ne fix(DATA)) then test_status = FAIL

if (test_status eq PASS) then begin
    print, 'SUCCESS'
end else begin
    print, 'FAILURE'
endelse

''',
'''

% Compiled module: MDSVALUE.
% Compiled module: MDSCHECKARG.
% Compiled module: MDSISCLIENT.
% Compiled module: MDS_KEYWORD_SET.
% Compiled module: MDSIDLIMAGE.
result=          55
% Compiled module: MDSCONNECT.
% Compiled module: MDSDISCONNECT.
result=          55
SUCCESS

''')


#-------------------------------------------------------------------------------
# https://github.com/MDSplus/mdsplus/issues/2639
# Issue #2639: mdsvalue, one connect, and kill 0 socket.
# Note different behavior from IDL-2639-kill-single-socket test.
# idl_test('IDL-2639-kill-single-zero',
# f'''

# PASS = 1
# FAIL = 0
# DATA = '55'
# mdsip_server = '{args.mdsip_server}'
# test_status = PASS

# result = mdsvalue(DATA)
# print, 'result=', result
# if (result ne fix(DATA)) then test_status = FAIL

# mdsconnect, mdsip_server
# mdsdisconnect, socket=0

# result = mdsvalue(DATA)
# print, 'result=', result
# if (result ne fix(DATA)) then test_status = FAIL

# if (test_status eq PASS) then begin
#     print, 'SUCCESS'
# end else begin
#     print, 'FAILURE'
# endelse

# ''',
# '''

# % Compiled module: MDSVALUE.
# % Compiled module: MDSCHECKARG.
# % Compiled module: MDSISCLIENT.
# % Compiled module: MDS_KEYWORD_SET.
# % Compiled module: MDSIDLIMAGE.
# result=          55
# % Compiled module: MDSCONNECT.
# % Compiled module: MDSDISCONNECT.
# result=          55
# SUCCESS

# ''')


#-------------------------------------------------------------------------------
# https://github.com/MDSplus/mdsplus/issues/2640
# Issue #2640: disconnect returns correct status.
# First disconnect should succeed and thus return True (1).
# But disconnecting an already disconnected socket should return False (0).
# idl_test('IDL-2640-status',
# f'''

# PASS = 1
# FAIL = 0
# BOGUS = -77
# mdsip_server = '{args.mdsip_server}'
# test_status = PASS

# mdsconnect, mdsip_server
# print, '!MDS_SOCKET=', !MDS_SOCKET

# statusvar = BOGUS
# mdsdisconnect, socket=0, status=statusvar
# print, '!MDS_SOCKET, statusvar=', !MDS_SOCKET, statusvar
# if (statusvar ne 1) then test_status = FAIL

# statusvar = BOGUS
# mdsdisconnect, socket=0, status=statusvar
# print, '!MDS_SOCKET, statusvar=', !MDS_SOCKET, statusvar
# if (statusvar ne 0) then test_status = FAIL

# if (test_status eq PASS) then begin
#     print, 'SUCCESS'
# end else begin
#     print, 'FAILURE'
# endelse

# ''',
# '''

# % Compiled module: MDSCONNECT.
# % Compiled module: MDS_KEYWORD_SET.
# % Compiled module: MDSDISCONNECT.
# !MDS_SOCKET=       0
# !MDS_SOCKET, statusvar=       0       1
# !MDS_SOCKET, statusvar=       0       0
# SUCCESS

# ''')


if args.database_name != '':


    #---------------------------------------------------------------------------
    # Database: dbdisconnect kills database proxy
    idl_test('IDL-db-dbdisconnect',
    f'''

    PASS = 1
    FAIL = 0
    BOGUS = -77
    proxy = '{args.database_name}'
    mdsip_server = '{args.mdsip_server}'
    query = 'select getdate()'
    test_status = PASS

    set_database, proxy
    print, '!MDSDB_SOCKET=', !MDSDB_SOCKET
    val1 = BOGUS
    d = dsql(query, val1)
    if (typename(val1) ne "STRING") then test_status = FAIL

    mdsdbdisconnect
    print, '!MDSDB_SOCKET=', !MDSDB_SOCKET
    if (!MDSDB_SOCKET ne -1) then test_status = FAIL
    val2 = BOGUS
    d = dsql(query, val2)
    print, 'val2=', val2
    if ((typename(val2) ne "INT") || (val2 ne BOGUS)) then test_status = FAIL

    if (test_status eq PASS) then begin
        print, 'SUCCESS'
    end else begin
        print, 'FAILURE'
    endelse

    ''',
    '''

    % Compiled module: SET_DATABASE.
    % Compiled module: MDS_KEYWORD_SET.
    % Compiled module: MDSVALUE.
    % Compiled module: MDSCHECKARG.
    % Compiled module: MDSISCLIENT.
    % Compiled module: DSQL.
    !MDSDB_SOCKET=       0
    % Compiled module: EVALUATE.
    % Compiled module: MDSDISCONNECT.
    !MDSDB_SOCKET=      -1
    % Compiled module: MDSIDLIMAGE.
    % DSQL: SET_DATABASE must preceed any DSQL calls
    val2=     -77
    SUCCESS

    ''')


    #---------------------------------------------------------------------------
    # Database: regular disconnect kills database proxy.
    # GA usually has mdsconnect followed by set_database.
    # Note different behavior compared to IDL-db-dbdisconnect test.
    idl_test('IDL-db-disconnect',
    f'''

    PASS = 1
    FAIL = 0
    BOGUS = -77
    proxy = '{args.database_name}'
    mdsip_server = '{args.mdsip_server}'
    query = 'select getdate()'
    test_status = PASS

    mdsconnect, mdsip_server
    print, '!MDS_SOCKET=', !MDS_SOCKET

    set_database, proxy
    print, '!MDSDB_SOCKET=', !MDSDB_SOCKET
    val1 = BOGUS
    d = dsql(query, val1)
    if (typename(val1) ne "STRING") then test_status = FAIL

    mdsdisconnect, socket=1
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne 0) || (!MDSDB_SOCKET ne 1)) then test_status = FAIL
    val2 = BOGUS
    d = dsql(query, val2)
    print, 'val2=', val2
    if ((typename(val2) ne "INT") || (val2 ne 0)) then test_status = FAIL

    if (test_status eq PASS) then begin
        print, 'SUCCESS'
    end else begin
        print, 'FAILURE'
    endelse

    ''',
    '''

    % Compiled module: MDSCONNECT.
    % Compiled module: MDS_KEYWORD_SET.
    % Compiled module: MDSDISCONNECT.
    !MDS_SOCKET=       0
    % Compiled module: SET_DATABASE.
    % Compiled module: MDSVALUE.
    % Compiled module: MDSCHECKARG.
    % Compiled module: MDSISCLIENT.
    % Compiled module: DSQL.
    !MDSDB_SOCKET=       1
    % Compiled module: EVALUATE.
    !MDS_SOCKET, !MDSDB_SOCKET=       0       1
    % MDSVALUE: Error evaluating expression
    val2=       0
    SUCCESS

    ''')


    #***************************************************************************
    # The following "socket" tests establish a baseline for the current code.
    # The whole concept of the IDL API (!MDS_SOCKET, !MDSDB_SOCKET, etc.) doesn't
    # reflect the full features of the underlying C code.  And thus IDL sockets
    # are apt to be sensitive to any changes made to the IDL API.
    #***************************************************************************


    #---------------------------------------------------------------------------
    # Sockets: a sequence of socket operations.
    idl_test('IDL-socket-sequence',
    f'''

    PASS = 1
    FAIL = 0
    BOGUS = -77
    proxy = '{args.database_name}'
    mdsip_server = '{args.mdsip_server}'
    query = 'select getdate()'
    test_status = PASS

    set_database, proxy
    print, '!MDSDB_SOCKET=', !MDSDB_SOCKET
    if (!MDSDB_SOCKET ne 0) then test_status = FAIL

    mdsconnect, mdsip_server
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne 1) || (!MDSDB_SOCKET ne 0)) then test_status = FAIL

    mdsconnect, mdsip_server
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne 2) || (!MDSDB_SOCKET ne 0)) then test_status = FAIL

    set_database, proxy
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne 2) || (!MDSDB_SOCKET ne 3)) then test_status = FAIL

    mdsdisconnect, socket=1
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne 2) || (!MDSDB_SOCKET ne 3)) then test_status = FAIL

    mdsconnect, mdsip_server
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne 4) || (!MDSDB_SOCKET ne 3)) then test_status = FAIL

    mdsdbdisconnect
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne 4) || (!MDSDB_SOCKET ne -1)) then test_status = FAIL

    set_database, proxy
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne 4) || (!MDSDB_SOCKET ne 5)) then test_status = FAIL

    mdsconnect, mdsip_server
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne 6) || (!MDSDB_SOCKET ne 5)) then test_status = FAIL

    if (test_status eq PASS) then begin
        print, 'SUCCESS'
    end else begin
        print, 'FAILURE'
    endelse

    ''',
    '''

    % Compiled module: SET_DATABASE.
    % Compiled module: MDS_KEYWORD_SET.
    % Compiled module: MDSVALUE.
    % Compiled module: MDSCHECKARG.
    % Compiled module: MDSISCLIENT.
    % Compiled module: DSQL.
    !MDSDB_SOCKET=       0
    % Compiled module: MDSDISCONNECT.
    !MDS_SOCKET, !MDSDB_SOCKET=       1       0
    !MDS_SOCKET, !MDSDB_SOCKET=       2       0
    !MDS_SOCKET, !MDSDB_SOCKET=       2       3
    !MDS_SOCKET, !MDSDB_SOCKET=       2       3
    !MDS_SOCKET, !MDSDB_SOCKET=       4       3
    !MDS_SOCKET, !MDSDB_SOCKET=       4      -1
    !MDS_SOCKET, !MDSDB_SOCKET=       4       5
    !MDS_SOCKET, !MDSDB_SOCKET=       6       5
    SUCCESS

    ''')


    #---------------------------------------------------------------------------
    # Sockets: reset of the !MDS* system variables
    idl_test('IDL-socket-reset',
    f'''

    PASS = 1
    FAIL = 0
    BOGUS = -77
    proxy = '{args.database_name}'
    mdsip_server = '{args.mdsip_server}'
    query = 'select getdate()'
    test_status = PASS

    mdsconnect, mdsip_server
    print, '!MDS_SOCKET=', !MDS_SOCKET
    if (!MDS_SOCKET ne 0) then test_status = FAIL

    set_database, proxy
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne 0) || (!MDSDB_SOCKET ne 1)) then test_status = FAIL

    mdsdisconnect
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne -1) || (!MDSDB_SOCKET ne 1)) then test_status = FAIL

    mdsdbdisconnect
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne -1) || (!MDSDB_SOCKET ne -1)) then test_status = FAIL

    if (test_status eq PASS) then begin
        print, 'SUCCESS'
    end else begin
        print, 'FAILURE'
    endelse

    ''',
    '''

    % Compiled module: MDSCONNECT.
    % Compiled module: MDS_KEYWORD_SET.
    % Compiled module: MDSDISCONNECT.
    !MDS_SOCKET=       0
    % Compiled module: SET_DATABASE.
    % Compiled module: MDSVALUE.
    % Compiled module: MDSCHECKARG.
    % Compiled module: MDSISCLIENT.
    % Compiled module: DSQL.
    !MDS_SOCKET, !MDSDB_SOCKET=       0       1
    !MDS_SOCKET, !MDSDB_SOCKET=      -1       1
    !MDS_SOCKET, !MDSDB_SOCKET=      -1      -1
    SUCCESS

    ''')


    #---------------------------------------------------------------------------
    # Sockets: explicitly reset socket 0.
    # Note different behavior than IDL-socket-reset test.
    idl_test('IDL-socket-reset-zero',
    f'''

    PASS = 1
    FAIL = 0
    BOGUS = -77
    proxy = '{args.database_name}'
    mdsip_server = '{args.mdsip_server}'
    query = 'select getdate()'
    test_status = PASS

    mdsconnect, mdsip_server
    print, '!MDS_SOCKET=', !MDS_SOCKET
    if (!MDS_SOCKET ne 0) then test_status = FAIL

    set_database, proxy
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne 0) || (!MDSDB_SOCKET ne 1)) then test_status = FAIL

    mdsdisconnect, socket=0
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne 0) || (!MDSDB_SOCKET ne 1)) then test_status = FAIL

    mdsdbdisconnect
    print, '!MDS_SOCKET, !MDSDB_SOCKET=', !MDS_SOCKET, !MDSDB_SOCKET
    if ((!MDS_SOCKET ne 0) || (!MDSDB_SOCKET ne -1)) then test_status = FAIL

    if (test_status eq PASS) then begin
        print, 'SUCCESS'
    end else begin
        print, 'FAILURE'
    endelse

    ''',
    '''

    % Compiled module: MDSCONNECT.
    % Compiled module: MDS_KEYWORD_SET.
    % Compiled module: MDSDISCONNECT.
    !MDS_SOCKET=       0
    % Compiled module: SET_DATABASE.
    % Compiled module: MDSVALUE.
    % Compiled module: MDSCHECKARG.
    % Compiled module: MDSISCLIENT.
    % Compiled module: DSQL.
    !MDS_SOCKET, !MDSDB_SOCKET=       0       1
    !MDS_SOCKET, !MDSDB_SOCKET=       0       1
    !MDS_SOCKET, !MDSDB_SOCKET=       0      -1
    SUCCESS

    ''')


#---------------------------------------------------------------------------
# Read: various permutations of reading data from a tree
idl_test('IDL-read-various',
f'''

mdsconnect, '{args.mdsip_server}'
mdsopen, '{args.tree}', {args.shot}
print, mdsvalue('{args.text}')
print, mdsvalue('{args.numeric}')
print, mdsvalue('{args.signal}')
print, mdsvalue('{args.fullpath}')
mdsset_def, '{args.relative_def}'
print, mdsvalue('{args.relative}')
mdsset_def, '{args.reset_def}'
print, mdsvalue('{args.tag}')
print, mdsvalue('{args.wildcard}')
print, mdsvalue('{args.getnci}')
print, mdsvalue('{args.rank}')
print, mdsvalue('{args.ndims}')
print, mdsvalue('{args.dim_of}')
print, mdsvalue('{args.units_of}')
mdsclose, '{args.tree}', '{args.shot}'

''',
f'''

% Compiled module: MDSCONNECT.
% Compiled module: MDS_KEYWORD_SET.
% Compiled module: MDSDISCONNECT.
% Compiled module: MDSOPEN.
% Compiled module: MDSVALUE.
% Compiled module: MDSCHECKARG.
% Compiled module: MDSISCLIENT.
{args.text_value}
{args.numeric_value}
{args.signal_value}
{args.fullpath_value}
% Compiled module: MDSSET_DEF.
% Compiled module: MDSSETDEFAULT.
{args.relative_value}
{args.tag_value}
{args.wildcard_value}
{args.getnci_value}
{args.rank_value}
{args.ndims_value}
{args.dim_of_value}
{args.units_of_value}
% Compiled module: MDSCLOSE.

''')


# TODO: Allow for threading, use mdsip
WRITE_TREE_DIR = tempfile.TemporaryDirectory(prefix='test_idl_', dir='/tmp')
os.environ['default_tree_path'] = WRITE_TREE_DIR.name
build_write_tree(args.write_tree, args.write_shot)

#---------------------------------------------------------------------------
# Write: various permutations of writing data to a tree
#
# Note: Uses local tree on the build server (i.e., not using mdsip).
#       The $default_tree_path must point to the "idl/testing" directory.
idl_test('IDL-write-various',
f'''

mdsopen, '{args.write_tree}', '{args.write_shot}'
mdsput, 'A_TEXT', ' "string_a" '
mdsput, 'B_NUM', '22'
mdsput, 'C_SIGNAL', 'build_signal([10,-10,5,-5,0],[10.2,-10.2,5.4,-5.4,0.0], [0 .. 4])'
mdsput, '\TOP.SUBTREE_1:D_TEXT', ' "string_d" '
mdsput, 'SUBTREE_1.E_UNITS', 'build_with_units(55, "volts")'
mdsset_def, 'SUBTREE_1'
mdsput, '.-.SUBTREE_2:F_NUM', '66'
mdsput, '\TAG_G', '77'
mdsclose, '{args.write_tree}', '{args.write_shot}'

mdsopen, '{args.write_tree}', '{args.write_shot}'
print, mdsvalue('A_TEXT')
print, mdsvalue('B_NUM')
print, mdsvalue('DATA(C_SIGNAL)')
print, mdsvalue('RAW_OF(C_SIGNAL)')
print, mdsvalue('DIM_OF(C_SIGNAL)')
print, mdsvalue('SUBTREE_1:D_TEXT')
print, mdsvalue('SUBTREE_1.E_UNITS')
print, mdsvalue('UNITS_OF(SUBTREE_1.E_UNITS)')
print, mdsvalue('SUBTREE_2:F_NUM')
print, mdsvalue('SUBTREE_2:G_NUM')
mdsclose, '{args.write_tree}', '{args.write_shot}'

''',
'''

% Compiled module: MDSOPEN.
% Compiled module: MDSVALUE.
% Compiled module: MDSCHECKARG.
% Compiled module: MDSISCLIENT.
% Compiled module: MDS_KEYWORD_SET.
% Compiled module: MDSIDLIMAGE.
% Compiled module: MDSPUT.
% Compiled module: MDSSET_DEF.
% Compiled module: MDSSETDEFAULT.
% Compiled module: MDSCLOSE.
string_a
22
10         -10           5          -5           0
10.2000     -10.2000      5.40000     -5.40000      0.00000
0           1           2           3           4
string_d
55
volts
66
77

''')

all_tests_passed = True
for test in all_tests:
    test.join()

    if not test.passed:
        print(f'Test {test.name} failed, see {test.logfile}')
        all_tests_passed = False

if not all_tests_passed:
    exit(1)

