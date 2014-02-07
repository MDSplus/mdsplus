
function [ result, status ] = mdsvalue( expression, varargin)
%mdsvalue - evaluate an expression
%   
%
  
  import MDSplus.Data
  global MDSplus_Connection_Obj

  args = javaArray('MDSplus.Data',1);
  for k = 1: size(varargin, 2)
    args(k) = MDSarg(cell2mat(varargin(k)));
  end

  try
    if isjava(MDSplus_Connection_Obj)
      if size(varargin,2) > 0
        result = MDSplus_Connection_Obj.get(expression,args);
      else
        result = MDSplus_Connection_Obj.get(expression);
      end
    else
      result = Data.execute(expression, args);
    end
    status=1;
  catch err
    status=0;
    result=err.message;
  end
  result=NATIVEvalue(result);
end
