function status = mdsclose()
% function to mdsclose on remote server
% eg : mdsclose(para);
% if para ~= 0, perform a disconnect of remote server
% Basil P. DUVAL, Oct 1998

%status = mdsremote(3);
%status = mdsvalue(3);
%status  = mdsvalue('MDSLIB->MDS$CLOSE()');
status  = mdsvalue('TreeClose()');
