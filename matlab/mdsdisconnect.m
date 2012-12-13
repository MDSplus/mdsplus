function [ status ] = mdsdisconnect( )
%mdsdisconnect - disconnect from  a remote mdsplus data server. 
%   
%      mdsdisconnect will destroy this connection, reverting the above
%      described routines to their local behaviors
%
  
  import MDSplus.*
  global connection
   
   status = 1;
   if isa(connection, 'char')
       status = mdsvalue('mdsdisconnect()');
       clearvars -global connection;
   end
end

