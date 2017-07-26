function mdsUseLegacy( varargin )
%mdsUseLegacy - enable (or disable) the use of the legacy mode.
%   This function is can be used to enable legacy mode.
%   In legacy mode floating point values will always be returned as doubles.
%   Also the warnings about the deprecated functions will be disabled.
%   Legacy mode will be enabled if mdsUseLegacy is called with no arguments.
%   If you want to turn off legacy mode use mdsUseLegacy(false).
%   This is not a permanent setting and only applies to the current
%   invocation of matlab.
   mdsInfo();
   global MDSINFO
   if nargin == 0
     MDSINFO.useLegacy=true;
   else
     MDSINFO.useLegacy=varargin{1};
   end
   if MDSINFO.useLegacy
     display('Legacy mode enabled. Floating point values will be returned as doubles and MDSarg and NATIVEvalue will not display deprecation warnings. It is recommended that you modify your code to not depend on this legacy mode as it will eventually be deprecated.')
   end
end

