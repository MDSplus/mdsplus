function status = mdsconnect(server)
% function to mdsconnect to a server
% eg : mdsconnect(para);
% Basil P. DUVAL, Aug 2000
a=findstr(server,':');
if(length(a))
   server = [server(1:a(1)-1),'::',server(a(1):end)];
else
   server = [server,'::'];
end
   status = mdsopen(server);
