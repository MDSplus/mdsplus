function mdsUsePython( varargin )
%mdsUsePython - enable (or disable) the use of the python MDSplus bridge.
%   This function is can be used to select the python bridge instead of the
%   default java bridge. It will enable the use of python if called with
%   no arguments or use mdsUsePython(false) to revert back to the java
%   bridge. This is not a permanent setting and only applies to the current
%   invocation of matlab.
   mdsInfo();
   global MDSINFO
   if nargin == 0
     MDSINFO.usePython=true;
   else
     MDSINFO.usePython=varargin{1};
   end
   if MDSINFO.usePython
     try
       py_MDSplus_Int32=str2func('py.MDSplus.Int32');
       x=py_MDSplus_Int32(int32(1));
     catch
         display('The python bridge to MDSplus is not available. Reverting to java brdige.')
         MDSINFO.usePython=false;
     end
  end
end

