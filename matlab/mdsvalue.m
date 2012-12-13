
function [ result, status ] = mdsvalue( expression, varargin)
%mdsvalue - connect to a remote mdsplus data server 
%   
%      This routine will make a thin client connection to the specified
%      mdsplus data server.  It will cause subsequent invocations of 
%      mdsopen, mdsvalue, mdsput, and mdsclose to be executed remotely
%      on the specified host.
%
%      mdsdisconnect will destroy this connection, reverting the above
%      described routines to their local behaviors
%
  
   import MDSplus.*

   global connection
   
   status = -1;
   
   if  ~isa(connection, 'char')
       args = javaArray('MDSplus.Data',1);
       status = Data.execute('mdsconnect("LOCAL")', args);
       if status == -1 
           connection = 'LOCAL';
       end
   end
   if status == -1
       args = javaArray('MDSplus.Data',1);
       args(1) = MDSarg(expression);
       for k = 1: size(varargin, 2)
           args(k+1) = MDSarg(cell2mat(varargin(k)));
       end
       status = 0;
       expr = strcat('mdsvalue($',repmat(',$', 1,size(varargin, 2)),')');
       result = Data.execute(expr, args);
       result = NATIVEvalue(result);
   end
end

