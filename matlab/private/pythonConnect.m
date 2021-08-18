function connection = pythonConnect( host )
py_MDSplus_Connection = str2func('py.MDSplus.Connection');
connection = py_MDSplus_Connection(host);
