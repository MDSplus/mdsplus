function err = pythonActivate()
global MDSINFO
persistent cache
if ~isempty(cache)
    err = cache;
else
    try
        if MDSINFO.usemdsthin
            MDSINFO.ispy2 = false
        else
            MDSINFO.ispy2 = logical(py.MDSplus.version.ispy2);
        end
        err = false;
        cache = err;
    catch err
    end
end
