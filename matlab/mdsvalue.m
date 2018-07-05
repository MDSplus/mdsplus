
function [ result, status ] = mdsvalue( expression, varargin)
%mdsvalue - evaluate an expression
%   
%
  info=mdsInfo();
  if info.usePython
    n = nargin-1;
    args=cell(n,1);
    for idx = 1:n
      args{idx}=mdsFromMatlab(varargin{idx});
    end
  else
    if info.isConnected
      extra=0;
    else
      extra=1;
    end
    n = size(varargin,2);
    args = javaArray('MDSplus.Data',max(1,n+extra));
    for k = 1: n
      argin=varargin(k);
      try
        arg = mdsFromMatlab(cell2mat(argin));
      catch
        arg = mdsFromMatlab(argin{1});
      end
      args(k) = arg;
    end
  end

  try
    if info.isConnected
      if n > 0
        if info.usePython
          result = info.connection.get(expression,args{:});
        else
          result = info.connection.get(expression,args);
        end
      else
        result = info.connection.get(expression);
      end
    else
      if info.usePython
        py_MDSplus_EXECUTE=str2func('py.MDSplus.EXECUTE');
        result = py_MDSplus_EXECUTE(expression,args{:}).evaluate();
      else
        dobj=javaObject('MDSplus.Data');
        result = dobj.execute(expression, args);
      end
    end
    status=1;
  catch err
    status=0;
    result=err.message;
  end
  result=mdsToMatlab(result);
end
