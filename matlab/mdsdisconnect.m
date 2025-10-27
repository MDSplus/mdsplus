function [ status ] = mdsdisconnect( )
% MDSDISCONNECT  disconnect from  a remote mdsplus data server.   
%      mdsdisconnect will destroy this connection, reverting the above
%      described routines to their local behaviors
    global MDSINFO
    if MDSINFO.usemdsthin
        MDSINFO.connection = [];
        MDSINFO.connectedHost = '';
        MDSINFO.isConnected = false;
        status = 1;
    else
        status = mdsconnect('local');
    end
end

