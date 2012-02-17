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
    ans=thing;
else
    switch class(thing)
        case 'double'
            fprintf('it is a double\n')
            result = Float64Array(reshape(thing,[],1),size(thing));
        case 'single'
            fprintf( 'it is a single\n')
            result = Float32Array(reshape(thing,[],1),size(thing));
        case 'int64'
            fprintf( 'it is an int64\n')
            result = Int64Array(reshape(thing,[],1),size(thing));
        case 'int32'
            fprintf( 'it is an int32\n')
            result = Int32Array(reshape(thing,[],1),size(thing));
        case 'int16'
            fprintf( 'it is an int16\n')
            result = Int16Array(reshape(thing,[],1),size(thing));
        case 'int8'
            fprintf( 'it is an int8\n')
            result = Int16Array(reshape(thing,[],1),size(thing));
        otherwise
            fprintf('it is a %s\n', class(thing))
            result = thing;
    end
end

