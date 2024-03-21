function result = mdstest(varargin)
% MDSTEST  test the MDSplus-MATLAB API (types and shapes)
%   This routine tests the various functions in the MDSplus
%   Matlab/Octave interface.
%   mdstest()   % tests current bridge
%   mdstest(0)  % tests java bridge
%   mdstest(1)  % tests python bridge

    mdsInfo(varargin{:}); % update MDSINFO
    result = mdscheck('1BU', 'uint8', [1, 1]);
    result = result && mdscheck('1WU', 'uint16', [1, 1]);
    result = result && mdscheck('1LU', 'uint32', [1, 1]);
    result = result && mdscheck('100000000000QU', 'uint64', [1, 1]);
    result = result && mdscheck('1.', 'single', [1, 1]);
    result = result && mdscheck('1D0', 'double', [1, 1]);
    result = result && mdscheck('"string test"', 'char', [1, 11]);
    result = result && mdscheck('BYTE_UNSIGNED(1:100)', 'uint8', [100, 1]);
    result = result && mdscheck('WORD_UNSIGNED(1:100)', 'uint16', [100, 1]);
    result = result && mdscheck('LONG_UNSIGNED(1:100)', 'uint32', [100, 1]);
    result = result && mdscheck('BYTE(1:100)', 'int8', [100, 1]);
    result = result && mdscheck('WORD(1:100)', 'int16', [100, 1]);
    result = result && mdscheck('1:100', 'int32', [100, 1]);
    result = result && mdscheck('QUADWORD(1:100)', 'int64', [100, 1]);
    result = result && mdscheck('set_range(10,2,5,1:100)', 'int32', [10, 2, 5]);
    result = result && mdscheck('FS_FLOAT(1:100)', 'single', [100, 1]);
    result = result && mdscheck('FT_FLOAT(1:100)', 'double', [100, 1]);
    result = result && mdscheck('set_range(10,2,5,FT_FLOAT(1:100))', 'double', [10, 2, 5]);
    result = result && mdscheck('$ : $', 'int32', [100, 1], int32(1), int32(100));
    result = result && mdscheck('$ == $', 'uint8', [1, 1], 1, 2);
    result = result && mdscheck('QUADWORD_UNSIGNED(1:100)', 'uint64', [100, 1]);
    result = result && mdscheck('["a","b","c","d"]', 'cell', [4, 1]);
    result = result && mdscheck('set_range(2,3,["a","b","c long string","d","e","f"])', 'cell', [2, 3]);
end

function result = mdscheck(exp, result_class, result_size, varargin)
    x = mdsvalue(exp, varargin{:});
    if ~strcmp(class(x), result_class)
        fprintf('Error %s: expected class %s got %s\n', exp, result_class, class(x))
        result = 0;
        return
    end
    try
        if ~all(size(x) == result_size)
            fprintf('Error %s: expected size %s got %s\n', exp, mat2str(result_size), mat2str(size(x)))
            result = 0;
            return
        end
    catch
        fprintf('Error %s: expected size %s got %s\n', exp, mat2str(result_size), mat2str(size(x)))
        result = 0;
        return
    end
    y = mdsvalue('$', x);
    if isa(x, 'cell')
        try
            result = strcmp(x, y);
            result = all(result(:));
            if ~result
                display(strcat('Error testing "', exp, '": string array does not match'))
            end
        catch err
            display(strcat('Error testing "', exp, '": ', err.message))
            rethrow(err)
        end
    else
        try
            result = x==y;
            result = all(result(:));
            if ~result
                fprintf('Error %s: input != output\n', exp)
                display(x);
                display(y);
            end
        catch err
            display(strcat('Error testing "', exp, '": ', err.message))
            rethrow(err)
        end
    end
end
