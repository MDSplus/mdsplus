function [ status ] = mdsput( node, expression, varargin)
%mdsput put data into MDSplus tree node
%   Detailed explanation goes here
info=mdsInfo();
if nargin == 2 && info.useLegacy
  status = mdsput(node,'$',expression);
else
  status = mdsvalue(sprintf('treeput($,$%s)',repmat(',$',1,nargin-2)),node,expression,varargin{:});
end
