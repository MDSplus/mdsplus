function [ status ] = mdsput( node, expression, varargin)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

import MDSplus.Data
global MDSplus_Connection_Obj

if nargin == 2
    status = mdsput(node,'$',expression);
else
    args = javaArray('MDSplus.Data',1);
    for k = 1: length(varargin)
        args(k) = MDSarg(cell2mat(varargin(k)));
    end
    try
        if isjava(MDSplus_Connection_Obj)
            MDSplus_Connection_Obj.put(node,expression,args);
            status = true;
        else
            status = mdsvalue(['TreePut($,$',repmat(',$', 1,size(varargin, 2)),')'],node,expression,varargin{:});
        end
    catch err
        if nargout==1
            status = false;
        else
            rethrow(err)
        end
    end
end
end
