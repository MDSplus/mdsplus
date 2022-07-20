function [ shoto, status ] = mdsopen( expt, shot )
% MDSOPEN  opend connect to a remote mdsplus data server.
%   This routine will invoke a treeopen(expt, shot)
%   expt may contain information about a remote server 'server::expt'

   status = 1;
   shoto = 'Failed';
   idx = strfind(expt, '::');
   if ~isempty(idx)
       host = expt(1 : idx - 1);
       ltree = expt(idx + 2 : end);
       status = mdsconnect(host);
    else
       ltree = expt;
   end
   if mod(status, 2)
     info = mdsInfo();
     if info.isConnected
       try
         info.connection.openTree(ltree, int32(shot));
         shoto = mdsToMatlab(info.connection.get('$shot'));
       catch err
         status = 0;
         shoto = err.message;
       end
     else
       status = mdsvalue('TreeOpen($, $)', ltree, int32(shot));
       if mod(status, 2)
         shoto = mdsvalue('$shot');
       end
     end
   end
end
