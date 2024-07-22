function result = javaFromMatlabStruct(value)
  if ~isstruct(value)
    throw(MException('MDSplus:javaFromMatlabStruct', 'only struct allowed')); 
  end

    if isscalar(value)
        fields = fieldnames(value);
        result = MDSplus.Dictionary();
        for fieldIdx = 1:length(fields)
            fieldName = fields{fieldIdx};
            fieldValue = value.(fieldName);
            result.setItem(MDSplus.String(fieldName), javaFromMatlab(fieldValue));
        end
    elseif isvector(value)
        result = MDSplus.List();
        numItems = length(value);
        for itemIdx = 1:numItems
            if isrow(value)
                result.append(javaFromMatlabStruct(value(itemIdx)));
            else
                temp = MDSplus.List();
                temp.append(javaFromMatlabStruct(value(itemIdx)));
                result.append(temp);
            end
        end
    elseif ismatrix(value)
        result = MDSplus.List();
        elemSize = size(value);
        for rowIdx = 1:elemSize(1)
            result.append(javaFromMatlabStruct(value(rowIdx,:)));
        end
    else
        result = MDSplus.List();
        elemSize = size(value);
        n = ndims(value);
        idx = cell(1,n);
        idx(:) = {':'};
        for currDim = 1:elemSize(end)
            idx{end} = currDim;
            result.append(javaFromMatlab(value(idx{:})));
        end
    end

end