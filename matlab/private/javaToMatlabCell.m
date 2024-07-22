function result = javaToMatlabCell(value)
    if ~isa(value, 'MDSplus.Apd')
        throw(MException('MDSplus:javaToMatlabCell', 'only MDSplus.Apd allowed')); 
    end
    fields = value.getDescs();
    numItems = length(fields);
    itemClasses = arrayfun(@class, fields, 'UniformOutput', false);
    result = {};
    for itemIdx = 1:numItems
        currElem = javaToMatlab(fields(itemIdx));
        if all(string(itemClasses)=='MDSplus.Apd') && numItems ~= 1
           
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
            result(:, itemIdx) = {currElem};
        end
    end
end