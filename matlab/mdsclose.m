function status = mdsclose(para)
% function to mdsclose on remote server
% eg : mdsclose(para);        % para is optional
% if para ~= 0, perform a disconnect of remote server
% Basil P. DUVAL, Oct 1998

%status = mdsremote(3);
%status = mdsipmex(3);
%status  = mdsipmex('MDSLIB->MDS$CLOSE()');
if(nargin < 1);para=0;end
if(para)
status = mdsdisconnect(0);
else
status  = mdsipmex('TreeClose()');
end
