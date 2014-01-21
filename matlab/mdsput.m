function [ status ] = mdsput( node, expression, varargin)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
   if nargin == 2
     status = mdsput(node, '$', expression)
   else
     expr = strcat('treeput($, $', repmat(',$', 1,size(varargin, 2)),')');
     status = mdsvalue(expr, node, expression, varargin{:});
   end
end

