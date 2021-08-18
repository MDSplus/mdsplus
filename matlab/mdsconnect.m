function [ status ] = mdsconnect( host )
% MDSCONNECT  connect to a remote mdsplus data server.
%      This routine will make a thin client connection to the specified
%      mdsplus data server.  It will cause subsequent invocations of
%      mdsopen, mdsvalue, mdsput, and mdsclose to be executed remotely
%      on the specified host.
%
%      mdsdisconnect will destroy this connection, reverting the above
%      described routines to their local behaviors
%
    global MDSINFO
    mdsInfo();
    if MDSINFO.isConnected && strcmp(MDSINFO.connectedHost, host) && MDSINFO.usePython == MDSINFO.isPythonConnection
        status = 1;
    else
        if strcmpi(host, 'LOCAL') == 1
            MDSINFO.isConnected = false;
            MDSINFO.connection = [];
            MDSINFO.connectedHost = host;
            status = 1;
        else
            try
                if MDSINFO.usePython
                    MDSINFO.connection = pythonConnect(host);
                else
                    MDSINFO.connection = javaConnect(host);
                end
                MDSINFO.isPythonConnection = MDSINFO.usePython;
                MDSINFO.connectedHost = host;
                MDSINFO.isConnected = true;
                status = 1;
            catch
                status = -1;
            end
        end
    end
end
