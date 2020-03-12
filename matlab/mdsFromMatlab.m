function  result = mdsFromMatlab( value )
% MDSFROMMATLAB  returns an MDSplus object created from a Matlab object
%	This function returns an MDSplus object representation of its input
%   argument.
%
%   syntax:
%       mdsresult=mdsFromMatlab(matlab-native-object)
%
    info = mdsInfo();
    if info.usePython
        result = matlab2python(value, info);
    else
        result = matlab2java(value, info);
    end
end

function result = matlab2python(value, info)
    dtype=class(value);
    switch dtype
        case 'char'
            result = value;
        otherwise
            switch dtype
                case 'single'
                    dtype='float32';
                case 'double'
                    dtype='float64';
                case 'cell'
                    dtype='str';
            end
            if info.ispy2 && contains(dtype,'int64')
                warning('MDSplus:python2_int64','%s\n',...
                    'Loss of precision: python2 does not support int64 properly. Try python3 instead.',...
                    'You can disable this warning with "warning(''off'', ''MDSplus:python2_int64'')"')
                value = double(value);
            end
            if isscalar(value)
                f=str2func(strcat('py.numpy.',dtype));
                result=f(value);
            else
                sz = size(value);
                result = py.numpy.array(reshape(value,int32(1),prod(int32(sz))),dtype);
                if not(numel(sz) == 2 && sz(2) == 1)
                    result = py.numpy.resize(result, flip(int32(sz)));
                    result = py.numpy.ascontiguousarray(result);
                end
            end
    end
end

function result = matlab2java(value, ~)
    switch class(value)
        case 'double'
            javaclass = 'MDSplus.Float64';
        case 'single'
            javaclass = 'MDSplus.Float32';
        case 'int64'
            javaclass = 'MDSplus.Int64';
        case 'uint64'
            javaclass = 'MDSplus.Uint64';
        case 'int32'
            javaclass = 'MDSplus.Int32';
        case 'uint32'
            javaclass = 'MDSplus.Uint32';
        case 'int16'
            javaclass = 'MDSplus.Int16';
        case 'uint16'
            javaclass = 'MDSplus.Uint16';
        case 'int8'
            javaclass = 'MDSplus.Int8';
        case 'uint8'
            javaclass = 'MDSplus.Uint8';
        case 'cell'
            javaclass = 'MDSplus.String';
        case 'char'
            result = javaObject('MDSplus.String',value);
            return
        otherwise
            result = value;
            return
    end
    sz = size(value);
    if isequal(sz, [1,1])
        result = javaObject(javaclass,value);
    else
        result = javaObject(strcat(javaclass,'Array'),reshape(value,[],1),sz);
    end
end
