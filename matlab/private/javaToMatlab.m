function result = javaToMatlab(mdsthing)
if ~strncmp(class(mdsthing), 'MDSplus', 7)
    result = mdsthing;
else
    if numel(mdsthing.getShape()) > 0
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
                result = reshape(mdsthing.getFloatArray,  shape);
            case 'MDSplus.StringArray'
                result = reshape(cellstr(string(mdsthing.getStringArray)), shape);
            case 'MDSplus.Dictionary'
                result = javaToMatlabStruct(mdsthing);
            case 'MDSplus.Apd'
                result = javaToMatlabStruct(mdsthing);
            otherwise
                throw(MException('MDSplus:mdsToMatlab', 'class %s not supported by mdsToMatlab function\n', class(mdsthing)));
        end
    else
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
                result = mdsthing.getFloatArray();
            case 'MDSplus.String'
                result = string(mdsthing.getString());
            otherwise
                throw(MException('MDSplus:mdsToMatlab', 'class %s not supported by mdsToMatlab function\n', class(mdsthing)));
        end
    end
end
