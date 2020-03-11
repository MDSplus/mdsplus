function obj = NATIVEvalue( value )
% NATIVEVALUE  deprecated in favour of mdsToMatlab
%   This function is deprecated. please use mdsToMatlab instead
    warning('MDSplus:Legacy',...
        'The NATIVEvalue function is deprecated. Please use mdsToMatlab instead')
    obj = mdsToMatlab(value);
end
