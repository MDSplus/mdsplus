function [ result, status ] = mdsvalue( expression, varargin)
%mdsvalue - evaluate an expression
%
%

import MDSplus.Data
global MDSplus_Connection_Obj

args = javaArray('MDSplus.Data',1);
for k = 1: length(varargin)
    args(k) = MDSarg(cell2mat(varargin(k)));
end

try
    if isjava(MDSplus_Connection_Obj)
        if ~isempty(varargin)
            result = MDSplus_Connection_Obj.get(expression,args);
        else
            result = MDSplus_Connection_Obj.get(expression);
        end
    else
        result = Data.execute(expression, args);
    end
    status = true;
catch err
    if nargout==2
        status = false;
        result = err.message;
    else
        rethrow(err)
    end
end
result = NATIVEvalue(result);