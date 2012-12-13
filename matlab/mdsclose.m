function [ status ] = mdsclose()
%mdsconnect - connect to a remote mdsplus data server. 
%   
%     This routine will close the tree at the top of the open tree stack.
% 
%      Previous incarnations also disconnected thin-client (mdsconnect) connections.
%      THAT IS NO LONGER DONE
%
   status = mdsvalue('TreeClose()');
end

