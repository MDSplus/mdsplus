function [ shoto, status ] = mdsopen( tree, shot )
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
import MDSplus.Data
global MDSplus_Connection_Obj
shoto  = 'Failed';
idx    = strfind(tree, '::');
if ~isempty(idx)
    host = tree(1:idx-1);
    ltree = tree(idx+2:end);
    status = mdsconnect(host);
else
    ltree = tree;
    status = true;
end
if status
    if isjava(MDSplus_Connection_Obj)
        try
            MDSplus_Connection_Obj.openTree(ltree,shot);
            shoto = MDSplus_Connection_Obj.get('$shot');
        catch err
            if nargout==2
                status = false;
                shoto  = err.message;
            else
                rethrow(err)
            end
        end
    else
        [result,status] = mdsvalue('TreeOpen($,$)',ltree,shot);
        status = status && mod(result,2);
        if status
            shoto = mdsvalue('$shot');
        end
    end
end
end

