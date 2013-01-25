function  result = MDSarg( thing )
%MDSarg - returns an object that can be passed to MDSplus.putData
%
% This function returns an MDSplus object representation of its input
% argument.  This way MDSplus.node.putData  can operate on and preserve
% the type and shape of its input arguments.
%
% syntax:
%   node.putData(MDSarg(matlab-native-object))
%
import MDSplus.*;
if size(thing) == [1,1]
    switch class(thing)
        case 'double'
            result = Float64(thing);
        case 'single'
            result = Float32(thing);
        case 'int64'
            result = Int64(thing);
        case 'int32'
            result = Int32(thing);
        case 'int16'
            result = Int16(thing);
        case 'int8'
            result = Int8(thing);
        case 'char'
            result = String(thing);
        otherwise
            result = thing;
    end
else
    sz = size(thing);
    switch class(thing)
        case 'double'
            result = Float64Array(reshape(thing,[],1),sz(sz > 1));
        case 'single'
            result = Float32Array(reshape(thing,[],1),sz(sz > 1));
        case 'int64'
            result = Int64Array(reshape(thing,[],1),sz(sz > 1));
        case 'int32'
             result = Int32Array(reshape(thing,[],1),sz(sz > 1));
        case 'int16'
            result = Int16Array(reshape(thing,[],1),sz(sz > 1));
        case 'int8'
            result = Int8Array(reshape(thing,[],1),sz(sz > 1));
        case 'char'
            result = String(thing);
        otherwise
            result = thing;
    end
end

