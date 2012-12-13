function [ status ] = mdsput( node, expression, varargin)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

   expr = strcat('mdsput($, $', repmat(',$', 1,size(varargin, 2)),')');
   status = mdsvalue(expr, node, expression, varargin{:});

end

