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
  
    import MDSplus.Connection
    global MDSplus_Connection_Host
    global MDSplus_Connection_Obj
   
    status = 0;
    if isa(MDSplus_Connection_Host, 'char')
      if compareIP(MDSplus_Connection_Host, host) == 1 
         status = 1;
      end
    end
    if status == 0
      if strcmp(upper(host),'LOCAL') == 1
        clearvars -global MDSplusConnection_Obj
        MDSplus_Connection_Host='LOCAL';
        status = 1;
      else
        try
          MDSplus_Connection_Obj=Connection(host);
          MDSplus_Connection_Host=host;
          status = 1;
        catch err
	  status = -1;
        end
      end
    end
end

