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
global MDSplus_legacy_behavior
if ~strcmp(class(MDSplus_legacy_behavior), 'logical')
    l = getenv('MDSplus_legacy_behavior')
    if strcmp(l,'yes')
        MDSplus_legacy_behavior = true
    else
	MDSplus_legacy_behavior = false
    end
end


 
if  isa(mdsthing, 'MDSplus.Data') == 0
    result = mdsthing;
else
    if isa (mdsthing, 'MDSplus.Array')
        shape = mdsthing.getShape;
        if numel(shape) == 1
            shape = [shape, 1];
        else
            shape = shape';
        end
            
        switch class(mdsthing)
            case 'MDSplus.Int64Array'
                result = reshape(mdsthing.getLongArray, shape);
            case 'MDSplus.Uint64Array'
                result = reshape(typecast(mdsthing.getLongArray, 'uint64'), shape);
            case 'MDSplus.Int32Array'
                result = reshape(mdsthing.getIntArray, shape);
            case 'MDSplus.Uint32Array'
                result = reshape(typecast(mdsthing.getIntArray, 'uint32'), shape);
            case 'MDSplus.Int16Array'
                result = reshape(mdsthing.getShortArray, shape);
            case 'MDSplus.Uint16Array'
                result = reshape(typecast(mdsthing.getShortArray, 'uint16'), shape);
            case 'MDSplus.Int8Array'
                result = reshape(mdsthing.getByteArray, shape);
            case 'MDSplus.Uint8Array'
                result = reshape(typecast(mdsthing.getByteArray, 'uint8'), shape);
            case 'MDSplus.Float64Array'
                result = double(reshape(mdsthing.getDoubleArray, shape));
            case 'MDSplus.Float32Array'
                result = double(reshape(mdsthing.getFloatArray,  shape));
            case 'MDSplus.StringArray'
		result = reshape(cellstr(char(mdsthing.getStringArray)),shape);
            otherwise
                throw(MException('MDSplus:NATIVEvalue', 'class %s not supported by NATIVEvalue function\n', class(mdsthing)));
        end
    elseif isa (mdsthing, 'MDSplus.Scalar')
        switch class(mdsthing)
            case 'MDSplus.Int64'
                result = mdsthing.getLongArray();
            case 'MDSplus.Uint64'
                result = typecast(mdsthing.getLongArray(), 'uint64');
            case 'MDSplus.Int32'
                result = int32(mdsthing.getIntArray());
            case 'MDSplus.Uint32'
                result = typecast(mdsthing.getIntArray(), 'uint32');
            case 'MDSplus.Int16'
                result = mdsthing.getShortArray();
            case 'MDSplus.Uint16'
                result = typecast(mdsthing.getShortArray(), 'uint16');
            case 'MDSplus.Int8'
                result = mdsthing.getByteArray();
            case 'MDSplus.Uint8'
                result = typecast(mdsthing.getByteArray(), 'uint8');
            case 'MDSplus.Float64'
                result = mdsthing.getDoubleArray();
            case 'MDSplus.Float32'
                result = double(mdsthing.getFloatArray());
            case 'MDSplus.String'
                result = char(mdsthing.getString());
            otherwise
                throw(MException('MDSplus:NATIVEvalue', 'class %s not supported by NATIVEvalue function\n', class(mdsthing)));
        end
        
    else
        throw(MException('MDSplus:NATIVEvalue', 'class %s not supported by NATIVEvalue function\n', class(mdsthing)));        
    end
    if MDSplus_legacy_behavior
        if ~strcmp(class(result),'char')
            result = double(result)
        end
    end 
end
