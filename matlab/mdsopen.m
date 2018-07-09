function [ shoto,status ] = mdsopen( tree, shot )
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
   status = 1;
   shoto = 'Failed';
   idx=strfind(tree, '::');
   if ~isempty(idx)
       host = tree(1:idx-1);
       ltree = tree(idx+2:end);
       status = mdsconnect(host);
    else
       ltree=tree;
   end
   if mod(status,2)
     info=mdsInfo();
     if info.isConnected
       try
         info.connection.openTree(ltree,int32(shot));
         shoto = mdsToMatlab(info.connection.get('$shot'));
       catch err
         status=0;
         shoto=err.message;
       end
     else
       status = mdsvalue('TreeOpen($,$)',ltree,int32(shot));
       if mod(status,2)
         shoto = mdsvalue('$shot');
       end
     end
   end
end

