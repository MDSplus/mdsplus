function result = javaExecute(expression, args)
dobj = javaObject('MDSplus.Data');
result = dobj.execute(expression, args);
