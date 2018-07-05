function  result = mdsFromMatlab( thing )
%mdsFromMatlab - returns an MDSplus object created from a Matlab object
%
% This function returns an MDSplus object representation of its input
% argument.
%
% syntax:
%   mdsresult=mdsFromMatlab(matlab-native-object)
%
  info=mdsInfo();
  if info.usePython
    dtype=class(thing);
    switch dtype
      case 'char'
        result=thing;
      otherwise
        switch dtype
          case 'single'
            dtype='float32';
          case 'double'
            dtype='float64';
          case 'cell'
            dtype='string';
        end
        if isscalar(thing)
          f=str2func(strcat('py.numpy.',dtype));
          result=f(thing);
	else
	  py_numpy_array=str2func('py.numpy.array');  
          sz=size(thing);
          result=py_numpy_array(reshape(thing,int32(1),prod(int32(size(thing)))),dtype);
          if not(numel(sz) == 2 && sz(2) == 1)
	    py_numpy_resize=str2func('py.numpy.resize');
            py_numpy_ascontiguousarray=str2func('py.numpy.ascontiguousarray');
            presult=py_numpy_resize(result,int32(sz));
            result=py_numpy_resize(result,presult.transpose().shape);
            result=py_numpy_ascontiguousarray(result);
          end
        end
    end
  else
    sz = size(thing);
    if isequal(sz, [1,1])
      switch class(thing)
        case 'double'
            result = javaObject('MDSplus.Float64',thing);
        case 'single'
            result = javaObject('MDSplus.Float32',thing);
        case 'int64'
            result = javaObject('MDSplus.Int64',thing);
        case 'uint64'
            result = javaObject('MDSplus.Uint64',thing);
        case 'int32'
            result = javaObject('MDSplus.Int32',thing);
        case 'uint32'
            result = javaObject('MDSplus.Uint32',thing);
        case 'int16'
            result = javaObject('MDSplus.Int16',thing);
        case 'uint16'
            result = javaObject('MDSplus.Uint16',thing);
        case 'int8'
            result = javaObject('MDSplus.Int8',thing);
        case 'uint8'
	          result = javaObject('MDSplus.Uint8',thing);
        case 'char'
            result = javaObject('MDSplus.String',thing);
        otherwise
            result = thing;
      end
    else
      switch class(thing)
        case 'double'
            result = javaObject('MDSplus.Float64Array',reshape(thing,[],1),sz);
        case 'single'
            result = javaObject('MDSplus.Float32Array',reshape(thing,[],1),sz);
        case 'int64'
            result = javaObject('MDSplus.Int64Array',reshape(thing,[],1),sz);
        case 'uint64'
            result = javaObject('MDSplus.Uint64Array',reshape(thing,[],1),sz);
        case 'int32'
             result = javaObject('MDSplus.Int32Array',reshape(thing,[],1),sz);
        case 'uint32'
             result = javaObject('MDSplus.Uint32Array',reshape(thing,[],1),sz);
        case 'int16'
            result = javaObject('MDSplus.Int16Array',reshape(thing,[],1),sz);
        case 'uint16'
            result = javaObject('MDSplus.Uint16Array',reshape(thing,[],1),sz);
        case 'int8'
            result = javaObject('MDSplus.Int8Array',reshape(thing,[],1),sz);
        case 'uint8'
            result = javaObject('MDSplus.Uint8Array',reshape(thing,[],1),sz);
        case 'char'
            result = javaObject('MDSplus.String',thing);
        case 'cell'
              result = javaObject('MDSplus.StringArray',reshape(thing,[],1),sz);
        otherwise
            result = thing;
      end
    end
  end
end

