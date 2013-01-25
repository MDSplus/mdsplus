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
        shape = mdsthing.getShape;
        if numel(shape) == 1
            shape = [1, shape];
        else
            shape = shape';
        end
            
        switch class(mdsthing)
            case 'MDSplus.Int64Array'
                result = reshape(mdsthing.getLongArray, shape);
            case 'MDSplus.Int32Array'
                result = reshape(mdsthing.getIntArray, shape);
            case 'MDSplus.Int16Array'
                result = reshape(mdsthing.getShortArray, shape);
            case 'MDSplus.Int8Array'
                result = reshape(mdsthing.getByteArray, shape);
            case 'MDSplus.Float64Array'
                result = reshape(mdsthing.getDoubleArray, shape);
            case 'MDSplus.Float32Array'
                result = reshape(mdsthing.getFloatArray,  shape);
            otherwise
                throw(MException('MDSplus:NATIVEvalue', 'class %s not supported by NATIVEvalue function\n', class(mdsthing)));
        end
    elseif isa (mdsthing, 'MDSplus.Scalar')
        switch class(mdsthing)
            case 'MDSplus.Int64'
                result = mdsthing.getLong;
            case 'MDSplus.Int32'
                result = mdsthing.getInt;
            case 'MDSplus.Int16'
                result = mdsthing.getShort;
            case 'MDSplus.Int8'
                result = mdsthing.getByte;
            case 'MDSplus.Float64'
                result = mdsthing.getDouble;
            case 'MDSplus.Float32'
                result = mdsthing.getFloat;
            case 'MDSplus.String'
                result = mdsthing.getString;
            otherwise
                throw(MException('MDSplus:NATIVEvalue', 'class %s not supported by NATIVEvalue function\n', class(mdsthing)));
        end
        
    else
        throw(MException('MDSplus:NATIVEvalue', 'class %s not supported by NATIVEvalue function\n', class(mdsthing)));
        
    end
end
