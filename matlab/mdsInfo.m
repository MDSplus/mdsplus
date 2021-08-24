function info = mdsInfo(varargin)
% MDSINFO  used internally by other functions to retrieve configuration info
global MDSINFO
if ~isempty(MDSINFO)
    info = MDSINFO;
else
    info.isConnected = false;
    info.connection = [];
    info.connectedHost = '';
    info.usePython = false;
    info.isPythonConnection = false;
    MDSINFO = info;
end
if nargin > 0
    MDSINFO.usePython = logical(varargin{1});
end
err = {true, true};
if MDSINFO.usePython
    try
        err{1} = pythonActivate();
    catch e
        err{1} = e;
    end
    if ~islogical(err{1})
        MDSINFO.usePython = false;
        err{2} = javaActivate();
    end
else
    err{2} = javaActivate();
    if ~islogical(err{2})
        MDSINFO.usePython = true;
        try
            err{1} = pythonActivate();
        catch e
            err{1} = e;
        end
    end
end
ok = cellfun(@islogical, err);
if ~all(ok)
    if ~any(ok)
        disp('Unable to connect to MDSplus using either a java bridge or a python bridge')
        disp(strcat(' Java error: ', err{2}.message))
        disp(strcat(' Python error: ', err{1}.message))
        info = [];
        MDSINFO = info;
    elseif ~ok(2)
        disp('Unable to connect to MDSplus using java bridge, using python bridge instead')
        disp(strcat(' Java error: ', err{2}.message))
    else % ~ok(1)
        disp('Unable to connect to MDSplus using python bridge, using python bridge instead')
        disp(strcat(' Python error: ', err{1}.message))
    end
end
