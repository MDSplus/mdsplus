function mdsUsePython( varargin )
% MDSUSEPYTHON  enable (or disable) the use of the python MDSplus bridge.
%   This function is can be used to select the python bridge instead of the
%   default java bridge. It will enable the use of python if called with
%   no arguments or use mdsUsePython(false) to revert back to the java
%   bridge. This is not a permanent setting and only applies to the current
%   invocation of matlab.
   if nargin == 0
       mdsInfo(true);
   else
       mdsInfo(varargin{1});
   end
end

