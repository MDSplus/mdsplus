function [ status ] = mdsput( node, expression, varargin)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

  import MDSplus.Data
  global MDSplus_Connection_Obj

  if nargin == 2
    status = mdsput(node,'$',expression);
  else
    args = javaArray('MDSplus.Data',1);
    for k = 1: size(varargin, 2)
      args(k) = MDSarg(cell2mat(varargin(k)));
    end
    try
      if isjava(MDSplus_Connection_Obj)
        MDSplus_Connection_Obj.put(node,expression,args);
        status=1;
      else
        status = mdsvalue(strcat('treeput($,$',repmat(',$', 1,size(varargin, 2)),')'),node,expression,varargin{:});
      end
    catch err
      status=0;
      err.message
    end
  end
end
