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
   
    status = false;
    if isa(MDSplus_Connection_Host, 'char')
      if compareIP(MDSplus_Connection_Host, host) 
         status = true;
      end
    end
    if ~status
      if strcmpi(host,'LOCAL')
        clearvars -global MDSplusConnection_Obj
        MDSplus_Connection_Host='LOCAL';
        status = true;
      else
        try
          MDSplus_Connection_Obj=Connection(host);
          MDSplus_Connection_Host=host;
          status = true;
        catch err
            if nargout==1
                status = false;
            else
                rethrow(err)
            end
        end
      end
    end
end
