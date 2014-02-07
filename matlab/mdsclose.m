function [ status ] = mdsclose()
%mdsconnect - connect to a remote mdsplus data server. 
%   
%     This routine will close the tree at the top of the open tree stack.
% 
%      Previous incarnations also disconnected thin-client (mdsconnect) connections.
%      THAT IS NO LONGER DONE
%
   import MDSplus.Data
   global MDSplus_Connection_Obj
   status = 1;
   shoto = '';
   if isjava(MDSplus_Connection_Obj)
     try
       MDSplus_Connection_Obj.get('TreeClose()');
     catch err
         status=0;
         err.message
     end
   else
     status = mdsvalue('TreeClose()');   
   end
end

