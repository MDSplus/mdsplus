function err = pythonActivate()
global MDSINFO
persistent cache
if ~isempty(cache)
    err = cache;
else
    try
        MDSINFO.ispy2 = logical(py.MDSplus.version.ispy2);
        err = false;
        cache = err;
    catch err
    end
end
