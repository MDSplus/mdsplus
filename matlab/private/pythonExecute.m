function result = pythonExecute(expression, args)
result = py.MDSplus.EXECUTE(expression, args{:});
result.evaluate();
