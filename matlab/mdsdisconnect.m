function [ status ] = mdsdisconnect( )
% MDSDISCONNECT  disconnect from  a remote mdsplus data server.   
%      mdsdisconnect will destroy this connection, reverting the above
%      described routines to their local behaviors
    status = mdsconnect('local');
end

