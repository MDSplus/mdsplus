function result = javaFromMatlab(value, ~)
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
    case 'struct'
        result = javaFromMatlabStruct(value);
        return
    case 'string'
        javaclass = 'MDSplus.String';
    case 'char'
        result = javaObject('MDSplus.String', value);
        return
    otherwise
        result = value;
        return
end
sz = size(value);
if isequal(sz, [1, 1])
    result = javaObject(javaclass, value);
else
    result = javaObject(strcat(javaclass, 'Array'), reshape(value, [], 1), sz);
end
