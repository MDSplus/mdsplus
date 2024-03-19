function result = pythonToMatlab(value, info)
if strncmp(class(value), 'py.MDSplus', 10)
    value = value.data();
end
if ~strncmp(class(value), 'py.numpy', 8)
    result = value;
else
    dtype = char(value.dtype.name);
    if contains(dtype, 'str') || contains(dtype, 'bytes')
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
                dtype = 'single';
            case 'float64'
                dtype = 'double';
        end
        f = str2func(dtype);
        dtype = value.dtype.char;
        if info.ispy2 && contains('Qq', char(dtype))
            warning('MDSplus:python2_int64', '%s\n', ...
                'Loss of precision: python2 does not support int64 properly. Try python3 instead.', ...
                'You can disable this warning with "warning(''off'', ''MDSplus:python2_int64'')"')
            dtype = 'd';
        end
        if value.ndim > 1
            shape = int64(py.array.array('i', value.shape));
            if ~py.numpy.isfortran(value)
                shape = flip(shape);
            end
            value = py.numpy.asfortranarray(value);
            value = py.array.array(dtype, value.flat);
            result = reshape(f(value), shape);
        else
            if value.ndim > 0
                result = reshape(f(py.array.array(dtype, value)), [int64(value.shape{1}), 1]);
            else
                result = f(py.array.array(dtype, {value}));
            end
        end
    end
end