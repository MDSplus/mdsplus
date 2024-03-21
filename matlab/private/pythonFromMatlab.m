function result = pythonFromMatlab(value, info)
dtype = class(value);
switch dtype
    case 'char'
        result = value;
    otherwise
        switch dtype
            case 'single'
                dtype = 'float32';
            case 'double'
                dtype = 'float64';
            case 'cell'
                dtype = 'str';
        end
        if info.ispy2 && contains(dtype, 'int64')
            warning('MDSplus:python2_int64', '%s\n', ...
                'Loss of precision: python2 does not support int64 properly. Try python3 instead.', ...
                'You can disable this warning with "warning(''off'', ''MDSplus:python2_int64'')"')
            value = double(value);
        end
        if isscalar(value)
            f = str2func(strcat('py.numpy.', dtype));
            result = f(value);
        else
            sz = size(value);
            result = py.numpy.array(reshape(value, int32(1), prod(int32(sz))), dtype);
            if not(numel(sz) == 2 && sz(2) == 1)
                result = py.numpy.resize(result, flip(int32(sz)));
                result = py.numpy.ascontiguousarray(result);
            end
        end
end
