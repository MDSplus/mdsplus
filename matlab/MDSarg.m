function obj = MDSarg( value )
% MDSARG  deprecated in favour of mdsFromMatlab.
%   This function is deprecated. please use mdsFromMatlab instead
    warning('MDSplus:Legacy',...
        'The MDSarg function is deprecated. Please use mdsFromMatlab instead')
    obj = mdsFromMatlab(value);
end

