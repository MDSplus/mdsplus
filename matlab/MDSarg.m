function obj = MDSarg( value )
%MDSarg deprecated function. New function is mdsFromMatlab.
%   This function is deprecated. please use mdsFromMatlab instead
  info=mdsInfo();
  if ~info.useLegacy
    display('The MDSarg function is deprecated. Please use mdsFromMatlab instead')
  end
  obj = mdsFromMatlab(value);
end

