function [ result] = PyExecute( expression, varargin)
% PyExecute evaluate an expression
%   [ result, status ] = mdsvalue( expression, varargin)
    info = mdsInfo();
    n = nargin-1;
    args=cell(n,1);
    result = py.MDSplus.EXECUTE(expression,args{:});
    result.evaluate();
    result=mdsToMatlab(result);
end
