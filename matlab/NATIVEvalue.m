function  result = NATIVEvalue( mdsthing )
%NATIVEvalue - returns a Native Matlab object from an MDSplus object
%   
% This Function takes an argument that is one of native scalar, native
% array, mdsplus scalar, or mdsplus array.  It returns a Native matlab 
% object of the corresponding type and shape.
%
% syntax:
%    answer = NATIVEvalue(mdsnode.getData)
%
import MDSplus.*;
if  isa(mdsthing, 'MDSplus.Data') == 0
    result = mdsthing;
else
    if isa (mdsthing, 'MDSplus.Array')
        switch class(mdsthing)
            case 'MDSplus.Int64Array'
                fprintf('it is an int64 array\n');
                result = reshape(mdsthing.getLongArray, mdsthing.getShape');
            case 'MDSplus.Int32Array'
                fprintf('it is an int32 array\n');
                result = reshape(mdsthing.getIntArray, mdsthing.getShape');
            case 'MDSplus.Int16Array'
                fprintf('itis an int16 Array\n');
                result = reshape(mdsthing.getShortArray, mdsthing.getShape');
            case 'MDSplus.Int8Array'
                fprintf('itis an int8 Array\n');
                result = reshape(mdsthing.getByteArray, mdsthing.getShape');
            case 'MDSplus.Float64Array'
                fprintf('it is a float64array\n');
                result = reshape(mdsthing.getDoubleArray,  mdsthing.getShape');
            case 'MDSplus.Float32Array'
                fprintf('it is a float32array\n');
                result = reshape(mdsthing.getFloatArray,  mdsthing.getShape');
            otherwise
                throw(MException('MDSplus:NATIVEvalue', 'class %s not supported by NATIVEvalue function\n', class(mdsthing)));
        end
    elseif isa (mdsthing, 'MDSplus.Scalar')
        switch class(mdsthing)
            case 'MDSplus.Int64'
                fprintf('it is an int64\n');
                result = mdsthing.getLong;
            case 'MDSplus.Int32'
                fprintf('it is an int32\n');
                result = mdsthing.getInt;
            case 'MDSplus.Int16'
                fprintf('itis an int16\n');
                result = mdsthing.getShort;
            case 'MDSplus.Int8Array'
                fprintf('itis an int8\n');
                result = mdsthing.getByte;
            case 'MDSplus.Float64Array'
                fprintf('it is a float64\n');
                result = mdsthing.getDouble;
            case 'MDSplus.Float32'
                fprintf('it is a float32\n');
                result = mdsthing.getFloat;
            case 'MDSplus.String'
                fprintf('it is a string\n')
                result = String(mdsthing);
            otherwise
                throw(MException('MDSplus:NATIVEvalue', 'class %s not supported by NATIVEvalue function\n', class(mdsthing)));
        end
        
    else
        throw(MException('MDSplus:NATIVEvalue', 'class %s not supported by NATIVEvalue function\n', class(mdsthing)));
        
    end
end

