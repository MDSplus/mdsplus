function  result = mdsFromMatlab( value )
% MDSFROMMATLAB  returns an MDSplus object created from a Matlab object
%	This function returns an MDSplus object representation of its input
%   argument.
%
%   syntax:
%       mdsresult = mdsFromMatlab(matlab_native_object)
%
    info = mdsInfo();
    if info.usePython
        result = pythonFromMatlab(value, info);
    else
        result = javaFromMatlab(value, info);
    end
end
