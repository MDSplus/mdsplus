function result = javaFromMatlabCell(value)
  if ~iscell(value)
    throw(MException('MDSplus:javaFromMatlabCell', 'only cell allowed')); 
  end  
  result = MDSplus.Apd();
    if isscalar(value)
        result.setDescAt(0, javaFromMatlab(value{:}));
    elseif isvector(value)
        numItems = length(value);
        for itemIdx = 1:numItems
            if isrow(value)
                result.setDescAt(itemIdx - 1, javaFromMatlab(value{itemIdx}));
            else
                result.setDescAt(itemIdx - 1, javaFromMatlab({value{itemIdx}}));
            end
        end
    elseif ismatrix(value)
        elemSize = size(value);
        for rowIdx = 1:elemSize(1)
            result.setDescAt(rowIdx - 1, javaFromMatlab(value(rowIdx,:)));
        end
    else
        elemSize = size(value);
        n = ndims(value);
        idx = cell(1,n);
        idx(:) = {':'};
        for currDim = 1:elemSize(end)
            idx{end} = currDim;
            result.setDescAt(currDim - 1, javaFromMatlab(value(idx{:})));
        end
    end

end