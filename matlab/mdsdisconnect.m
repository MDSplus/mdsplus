function status = mdsdisconnect()
% function to close communication with remote mds server
% eg : mdsdisconnect;
% Basil P. DUVAL, Oct 1998

%status = mdsremote(4);
status = mdsipmex(4);
