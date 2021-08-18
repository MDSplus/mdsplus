function  result = mdsToMatlab( mdsthing )
% MDSTOMATLAB  returns a Native Matlab object from an MDSplus object
% This Function takes an argument that is one of native scalar, native
% array, mdsplus scalar, or mdsplus array.  It returns a Native matlab
% object of the corresponding type and shape.
%
% syntax:
%    answer = mdsToMatlab(mdsnode.getData)
    info = mdsInfo();
    if info.usePython
        result = pythonToMatlab(mdsthing, info);
    else
        result = javaToMatlab(mdsthing);
    end
end
