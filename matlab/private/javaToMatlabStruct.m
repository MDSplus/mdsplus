function result = javaToMatlabStruct(value)
  if ~strcmp(class(value), 'MDSplus.Dictionary') &&  ~strcmp(class(value), 'MDSplus.Apd')
      throw(MException('MDSplus:javaToMatlabStruct', 'only MDSplus.Dictionary or MDSplus.Apd allowed')); 
  end
  fields = value.getDescs();
  if strcmp(class(value), 'MDSplus.Dictionary')
      numItems = length(fields)/2;
      for itemIdx = 1:numItems
        fieldName = javaToMatlab(fields(itemIdx * 2 - 1));
        fieldValue = javaToMatlab(fields(itemIdx * 2));
        result.(fieldName) = fieldValue;
      end
  else
      numItems = length(fields);
      for itemIdx = 1:numItems
          result(itemIdx) = javaToMatlab(fields(itemIdx));
      end
      
  end
   
end