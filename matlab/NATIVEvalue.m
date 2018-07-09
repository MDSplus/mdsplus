function [ obj ] = NATIVEvalue( value )
%NATIVEvalue deprecated function. New function is mdsToMatlab.
%   This function is deprecated. please use mdsToMatlab instead
  info=mdsInfo();
  if ~info.useLegacy
    display('The NATIVEvalue function is deprecated. Please use mdsToMatlab instead')
  end
  obj = mdsToMatlab(value);
end
