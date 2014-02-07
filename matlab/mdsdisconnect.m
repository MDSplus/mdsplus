function [ status ] = mdsdisconnect( )
%mdsdisconnect - disconnect from  a remote mdsplus data server. 
%   
%      mdsdisconnect will destroy this connection, reverting the above
%      described routines to their local behaviors
%
    global MDSplus_Connection_Host
    global MDSplus_Connection_Obj
    MDSplus_Connection_Host='LOCAL';
    clearvars -global MDSplus_Connection_Obj
    status=1; 
end

