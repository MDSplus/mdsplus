function  result = mdsToMatlab( mdsthing )
%mdsToMatlab - returns a Native Matlab object from an MDSplus object
%   
% This Function takes an argument that is one of native scalar, native
% array, mdsplus scalar, or mdsplus array.  It returns a Native matlab 
% object of the corresponding type and shape.
%
% syntax:
%    answer = mdsToMatlab(mdsnode.getData)
%
  info=mdsInfo();
  if info.usePython
    if strncmp(class(mdsthing),'py.MDSplus',10)
      value=mdsthing.data();
    else
      value=mdsthing;
    end
    if strncmp(class(value),'py.numpy',8) == 0 
      result = value;
    else
      dtype=char(value.dtype.name);
      if strfind(dtype,'str') == 1
        if value.ndim == 0
          result=char(value);
        else
          strs=value.flatten.tolist();
          result{length(strs)}='';
          for idx=1:length(strs)
             result{idx}=char(strs{idx});
          end
          shape=int32(py.array.array('i',value.shape));
          if numel(shape) == 1
            shape = [shape, int32(1)];
          else
            shape=int32(py.array.array('i',value.transpose().shape));
          end
          result = reshape(result,shape);
        end
      else
        switch dtype
          case 'float32'
            dtype='single';
          case 'float64'
            dtype='double';
        end
        f=str2func(dtype);
        if value.ndim > 1
	  py_numpy_transpose=str2func('py.numpy.transpose');
          py_numpy_asfortranarray=str2func('py.numpy.asfortranarray');
          shape=int32(py.array.array('i',py_numpy_transpose(value).shape));
          val=py_numpy_asfortranarray(value);
          pyarray=py.array.array(value.dtype.char,val.flat);
          result=reshape(f(py.array.array(value.dtype.char,pyarray)),shape);
        else
          if value.ndim > 0
            result=reshape(f(py.array.array(value.dtype.char,value)),[value.shape{1},1]);
          else
            result=f(py.array.array(value.dtype.char,{value}));
          end
        end
      end
    end
  else
    if strncmp(class(mdsthing),'MDSplus',7) == 0 
      result = mdsthing;
    else
      if numel(mdsthing.getShape()) > 0
        shape = mdsthing.getShape;
        if numel(shape) == 1
            shape = [shape, 1];
        else
            shape = shape';
        end
            
        switch class(mdsthing)
          case 'MDSplus.Int64Array'
            result = reshape(mdsthing.getLongArray, shape);
          case 'MDSplus.Uint64Array'
            result = reshape(typecast(mdsthing.getLongArray, 'uint64'), shape);
          case 'MDSplus.Int32Array'
            result = reshape(mdsthing.getIntArray, shape);
          case 'MDSplus.Uint32Array'
            result = reshape(typecast(mdsthing.getIntArray, 'uint32'), shape);
          case 'MDSplus.Int16Array'
            result = reshape(mdsthing.getShortArray, shape);
          case 'MDSplus.Uint16Array'
            result = reshape(typecast(mdsthing.getShortArray, 'uint16'), shape);
          case 'MDSplus.Int8Array'
            result = reshape(mdsthing.getByteArray, shape);
          case 'MDSplus.Uint8Array'
            result = reshape(typecast(mdsthing.getByteArray, 'uint8'), shape);
          case 'MDSplus.Float64Array'
            result = double(reshape(mdsthing.getDoubleArray, shape));
          case 'MDSplus.Float32Array'
            result = reshape(mdsthing.getFloatArray,  shape);
          case 'MDSplus.StringArray'
	        result = reshape(cellstr(char(mdsthing.getStringArray)),shape);
          otherwise
            throw(MException('MDSplus:mdsToMatlab', 'class %s not supported by mdsToMatlab function\n', class(mdsthing)));
        end
      else
        switch class(mdsthing)
          case 'MDSplus.Int64'
            result = mdsthing.getLongArray();
          case 'MDSplus.Uint64'
            result = typecast(mdsthing.getLongArray(), 'uint64');
          case 'MDSplus.Int32'
            result = int32(mdsthing.getIntArray());
          case 'MDSplus.Uint32'
            result = typecast(mdsthing.getIntArray(), 'uint32');
          case 'MDSplus.Int16'
            result = mdsthing.getShortArray();
          case 'MDSplus.Uint16'
            result = typecast(mdsthing.getShortArray(), 'uint16');
          case 'MDSplus.Int8'
            result = mdsthing.getByteArray();
          case 'MDSplus.Uint8'
            result = typecast(mdsthing.getByteArray(), 'uint8');
          case 'MDSplus.Float64'
            result = mdsthing.getDoubleArray();
          case 'MDSplus.Float32'
            result = mdsthing.getFloatArray();
          case 'MDSplus.String'
            result = char(mdsthing.getString());
          otherwise
            throw(MException('MDSplus:mdsToMatlab', 'class %s not supported by mdsToMatlab function\n', class(mdsthing)));
        end
      end
      if info.useLegacy
        if ~ischar(result)
          result = double(result);
        end
      end 
    end
  end
end
