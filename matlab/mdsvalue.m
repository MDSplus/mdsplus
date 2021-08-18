function [ result, status ] = mdsvalue( expression, varargin )
% MDSVALUE evaluate an expression
%   [ result, status ] = mdsvalue( expression, varargin )
info = mdsInfo();
n = nargin - 1;
if info.usePython
    args = cell(n, 1);
else
    if info.isConnected
        extra = 0;
    else
        extra = 1;
    end
    args = javaArray('MDSplus.Data', max(1, n + extra));
end
for k = 1 : n
    argin = varargin(k);
    if iscell(argin{1})
        argout = mdsFromMatlab(argin{1});
    else
        argout = mdsFromMatlab(cell2mat(argin));
    end
    if info.usePython
        args{k} = argout;
    else
        args(k) = argout;
    end
end

try
    if info.isConnected
        if n > 0
            if info.usePython
                result = info.connection.get(expression, args{:});
            else
                result = info.connection.get(expression, args);
            end
        else
            result = info.connection.get(expression);
        end
    else
        if info.usePython
            result = pythonExecute(expression, args);
        else
            result = javaExecute(expression, args);
        end
    end
    status = 1;
catch err
    status = 0;
    result = err.message;
end
result = mdsToMatlab(result);
