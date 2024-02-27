function result = javaFromMatlabStruct(value)
  if ~strcmp(class(value), 'struct')
    throw(MException('MDSplus:javaFromMatlabStruct', 'only struct allowed')); 
  end

    if isscalar(value)
        fields = fieldnames(value);
        result = MDSplus.Dictionary();
        for fieldIdx = 1:length(fields)
            fieldName = fields{fieldIdx};
            fieldValue = value.(fieldName);
            javaFromMatlab(fieldValue);
            result.setItem(MDSplus.String(fieldName), javaFromMatlab(fieldValue));
        end
    else
        result = MDSplus.Apd();
        numItems = length(value);
        for itemIdx = 1:numItems
           result.setDescAt(itemIdx - 1, javaFromMatlab(value(itemIdx))); 
        end
        
    end

end