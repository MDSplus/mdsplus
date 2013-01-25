function [ status ] = mdsconnect( host )
%mdsconnect - connect to a remote mdsplus data server. 
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
   
   status = 0;
   if isa(connection, 'char')
       if compareIP(connection, host) == 0 
           status = 1;
       end
   end
   if status == 0
       status = Data.execute(strcat('mdsconnect("',host, '")'), javaArray('MDSplus.Data', 1));
       if status == 1
           connection = host;
       else
           clearvars -global connection;
       end
   end
end

