
import os
import subprocess

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

all_tests_passed = True
def idl_test(code, expected_output):
    global all_tests_passed

    code_lines = [ line.strip() for line in code.splitlines() ]
    code_lines = list(filter(None, code_lines))
    code = '\n'.join(code_lines)

    code = 'pro test\n' + code + '\nend'
    open('test.pro', 'wt').write(code)

    expected_lines = [ line.strip() for line in expected_output.splitlines() ]
    expected_lines = list(filter(None, expected_lines))

    print('Running:')
    for line in code_lines:
        line = line.replace(server, '******')
        line = line.replace(dbname, '******')
        print(f'IDL> {line}')
    print()

    print('Expected:')
    for line in expected_lines:
        print(line)
    print()

    proc = subprocess.Popen(
        ['idl'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT
    )

    proc.stdin.write('test\nexit\n'.encode())
    proc.stdin.flush()
    
    hide_output = True

    print('Actual:')
    lines = []
    while True:
        line = proc.stdout.readline().decode()
        if not line:
            break

        line = line.strip()

        if not hide_output:
            print(line)
            if line != '':
                lines.append(line.strip())
        
        if line == '% Compiled module: TEST.':
            hide_output = False
    print()

    #stdout, _ = proc.communicate('test\n'.encode())
    #lines = [line.strip() for line in stdout.splitlines() ]
    lines = list(filter(None, lines))

    # Skip the IDL header and License information
    for i, line in enumerate(lines):
        if line == '% Compiled module: TEST.':
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


# Tree open / read / close
idl_test(f'''
         
testid = 'IDL-tree-read'
mdsconnect, '{server}'
mdsopen, '{tree}', {shot}
print, mdsvalue('{node1}')
print, mdsvalue('{node2}')
mdsclose, '{tree}', '{shot}'

''',
f'''

% Compiled module: MDSCONNECT.
% Compiled module: MDS_KEYWORD_SET.
% Compiled module: MDSDISCONNECT.
% Compiled module: MDSOPEN.
% Compiled module: MDSVALUE.
% Compiled module: MDSCHECKARG.
% Compiled module: MDSISCLIENT.
{node1_value}
{node2_value}
% Compiled module: MDSCLOSE.

''')


# Issue 2580: first connect should be on socket 0.
# Very rare for returned status to be zero.
idl_test(f'''
         
testid = 'IDL-2580-connect'         
PASS = 1
FAIL = 0
BOGUS = -77
mdsip_server = '{server}'
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

if dbname != '':

    # Issue #2625: database on socket 0, subsequent connect doesn't break proxy.
    # If the queries work, the "val" variables will be changed to a text timestamp.
    idl_test(f'''

    testid = "IDL-2625-simple"
    PASS = 1
    FAIL = 0
    BOGUS = -77                  
    proxy = '{dbname}'
    mdsip_server = '{server}'
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


    # Issue #2625: usual pattern at GA is connect, then database.
    idl_test(f'''

    testid = "IDL-2625-usual"
    PASS = 1
    FAIL = 0
    BOGUS = -77                  
    proxy = '{dbname}'
    mdsip_server = '{server}'
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


    # Issue #2625: second database call not affected.
    idl_test(f'''

    testid = "IDL-2625-two-database"
    PASS = 1
    FAIL = 0
    BOGUS = -77                  
    proxy = '{dbname}'
    mdsip_server = '{server}'
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


    # Issue #2625: stress test database proxy with many connects.
    # NLOOPS should be more than 64 (see Issue #2638).  If the
    # disconnect works, will never exceed the concurrent limit.
    idl_test(f'''

    testid = "IDL-2625-loop"
    PASS = 1
    FAIL = 0
    BOGUS = -77
    NLOOPS = 100                           
    proxy = '{dbname}'
    mdsip_server = '{server}'
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

# Issue #2638: crashes with too many concurrent sockets.
# NLOOPS should be more than 64.
idl_test(f'''

testid = "IDL-2638-loop"
PASS = 1
FAIL = 0
NLOOPS = 100
mdsip_server = '{server}'
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


# Issue #2639: mdsvalue works without a socket
idl_test(f'''
         
testid = 'IDL-2639-no-socket'
PASS = 1
FAIL = 0
DATA = '55'
mdsip_server = '{server}'
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


# Issue #2639: mdsvalue and interaction with killed socket.
# idl_test(f'''
         
# testid = 'IDL-2639-kill-last-socket'
# PASS = 1
# FAIL = 0
# DATA = '55'
# mdsip_server = '{server}'
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


# Issue #2639: mdsvalue and interaction with killed socket 0.
idl_test(f'''
         
testid = 'IDL-2639-kill-first-socket'
PASS = 1
FAIL = 0
DATA = '55'
mdsip_server = '{server}'
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


# Issue #2639: mdsvalue and kill default socket.
idl_test(f'''
         
testid = 'IDL-2639-kill-default-socket'
PASS = 1
FAIL = 0
DATA = '55'
mdsip_server = '{server}'
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


# Issue #2639: mdsvalue, one connect, and kill socket.
idl_test(f'''
         
testid = 'IDL-2639-kill-single-socket'
PASS = 1
FAIL = 0
DATA = '55'
mdsip_server = '{server}'
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


# Issue #2639: mdsvalue, one connect, and kill 0 socket.
# Note different behavior from IDL-2639-kill-single-socket test.
# idl_test(f'''
         
# testid = 'IDL-2639-kill-single-zero'
# PASS = 1
# FAIL = 0
# DATA = '55'
# mdsip_server = '{server}'
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


# Issue #2640: disconnect returns correct status.
# First disconnect should succeed and thus return True (1).
# But disconnecting an already disconnected socket should return False (0).
# idl_test(f'''
         
# testid = 'IDL-2640-status'
# PASS = 1
# FAIL = 0
# BOGUS = -77
# mdsip_server = '{server}'
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


if dbname != '':

    # Database: dbdisconnect kills database proxy
    idl_test(f'''
            
    testid = 'IDL-db-dbdisconnect'         
    PASS = 1
    FAIL = 0
    BOGUS = -77
    proxy = '{dbname}'
    mdsip_server = '{server}'
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


    # Database: regular disconnect kills database proxy.
    # GA usually has mdsconnect followed by set_database.
    # Note different behavior compared to IDL-db-dbdisconnect test.
    idl_test(f'''
            
    testid = 'IDL-db-disconnect'         
    PASS = 1
    FAIL = 0
    BOGUS = -77
    proxy = '{dbname}'
    mdsip_server = '{server}'
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

    #
    # The following "socket" tests establish a baseline for the current code.
    # The whole concept of the IDL API (!MDS_SOCKET, !MDSDB_SOCKET, etc.) doesn't
    # reflect the full features of the underlying C code.  And thus IDL sockets
    # are apt to be sensitive to any changes made to the IDL API.
    #

    # Sockets: a sequence of socket operations.
    idl_test(f'''
            
    testid = 'IDL-socket-sequence'         
    PASS = 1
    FAIL = 0
    BOGUS = -77
    proxy = '{dbname}'
    mdsip_server = '{server}'
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


    # Sockets: reset of the !MDS* system variables
    idl_test(f'''
            
    testid = 'IDL-socket-reset'         
    PASS = 1
    FAIL = 0
    BOGUS = -77
    proxy = '{dbname}'
    mdsip_server = '{server}'
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


    # Sockets: explicitly reset socket 0.
    # Note different behavior than IDL-socket-reset test.
    idl_test(f'''
            
    testid = 'IDL-socket-reset-zero'         
    PASS = 1
    FAIL = 0
    BOGUS = -77
    proxy = '{dbname}'
    mdsip_server = '{server}'
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

if not all_tests_passed:
    exit(1)

