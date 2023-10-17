function result = javaToMatlabStruct(value)
    if ~isa(value, 'MDSplus.Dictionary') &&  ~isa(value, 'MDSplus.List')
        throw(MException('MDSplus:javaToMatlabStruct', 'only MDSplus.Dictionary and MDSplus.List allowed')); 
    end
    fields = value.getDescs();
    if value.getSize() == 0
        result = struct();
    end
    if isa(value, 'MDSplus.Dictionary') % structure
        numItems = length(fields)/2;
        for itemIdx = 1:numItems
            fieldName = javaToMatlab(fields(itemIdx * 2 - 1));
            fieldValue = javaToMatlab(fields(itemIdx * 2));
            result.(fieldName) = fieldValue;
        end
    else
        numItems = length(fields);
        itemClasses = arrayfun(@class, fields, 'UniformOutput', false);
        for itemIdx =1:numItems
            if all(string(itemClasses)=='MDSplus.Dictionary') % list of dict mapped to struct array
                result(itemIdx) = javaToMatlabStruct(fields(itemIdx));
            elseif all(string(itemClasses)=='MDSplus.List') % struct matrix
                currElem = javaToMatlab(fields(itemIdx));
    
                % build the index
                if isscalar(currElem) || isvector(currElem)
                    numDims = 2;
                else
                    numDims = ndims(currElem) + 1;
                end
                idx = cell(1, numDims);
                idx(:) = {':'};
                if isscalar(currElem) || isvector(currElem)
                    idx{1} = itemIdx;
                else
                    idx{end} = itemIdx;
                end
    
                result(idx{:}) = currElem;
            else
                throw(MException('MDSplus:javaToMatlabStruct', 'incompatible types'));
            end
        end
    end
end