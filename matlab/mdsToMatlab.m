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
        result = python2matlab(mdsthing, info);
    else
        result = java2matlab(mdsthing, info);
    end
end

function result = python2matlab(value, info)
    if startsWith(class(value),'py.MDSplus')
        value = value.data();
    end
    if ~startsWith(class(value),'py.numpy')
        result = value;
    else
        dtype = char(value.dtype.name);
        if contains(dtype,'str') || contains(dtype,'bytes')
            if value.ndim == 0
                result = char(value);
            else
                strs = value.flatten.tolist();
                result{length(strs)} = '';
                for idx = 1 : length(strs)
                    result{idx} = char(py.MDSplus.version.tostr(strs{idx}));
                end
                shape = flip(int64(py.array.array('i', value.shape)));
                if numel(shape) == 1
                    shape = [shape, int64(1)];
                end
                result = reshape(result, shape);
            end
        else
            switch dtype
                case 'float32'
                    dtype='single';
                case 'float64'
                    dtype='double';
            end
            f=str2func(dtype);
            dtype = value.dtype.char;
            if info.ispy2 && contains('Qq',char(dtype))
                warning('MDSplus:python2_int64','%s\n',...
                    'Loss of precision: python2 does not support int64 properly. Try python3 instead.',...
                    'You can disable this warning with "warning(''off'', ''MDSplus:python2_int64'')"')
                dtype='d';
            end
            if value.ndim > 1
                shape = int64(py.array.array('i',value.shape));
                if ~py.numpy.isfortran(value)
                    shape = flip(shape);
                end
                value = py.numpy.asfortranarray(value);
                value = py.array.array(dtype,value.flat);
                result = reshape(f(value),shape);
            else
                if value.ndim > 0
                    result=reshape(f(py.array.array(dtype,value)),[int64(value.shape{1}),1]);
                else
                    result=f(py.array.array(dtype,{value}));
                end
            end
        end
    end
end

function result = java2matlab(mdsthing, info)
    if ~startsWith(class(mdsthing),'MDSplus')
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
                    result = reshape(cellstr(char(mdsthing.getStringArray)),shape);
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
                    result = char(mdsthing.getString());
                otherwise
                    throw(MException('MDSplus:mdsToMatlab', 'class %s not supported by mdsToMatlab function\n', class(mdsthing)));
            end
        end
        if info.useLegacy
            if ~ischar(result) && ~iscell(result)
                result = double(result);
            end
        end
    end
end
