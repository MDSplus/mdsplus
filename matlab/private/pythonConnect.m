function connection = pythonConnect( host )
global MDSINFO
if MDSINFO.usemdsthin
    py_MDSplus_Connection = str2func('py.mdsthin.Connection');
else
    py_MDSplus_Connection = str2func('py.MDSplus.Connection');
end
connection = py_MDSplus_Connection(host);
