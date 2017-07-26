function info = mdsInfo()
%mdsInfo - used internally by other functions to retrieve configuration info
%   
%
  global MDSINFO
  global MDSplus_legacy_behavior
  if isempty(MDSINFO)
    usePython=false;
    try
      MDSPLUS_DIR=getenv('MDSPLUS_DIR');
      if ismac
        java.lang.System.setProperty('JavaMdsLib',strcat(MDSPLUS_DIR,'/lib/libJavaMds.dylib'));
      elseif isunix
        java.lang.System.setProperty('JavaMdsLib',strcat(MDSPLUS_DIR,'/lib/libJavaMds.so'));
      elseif ispc
        java.lang.System.setProperty('JavaMdsLib','C:\WINDOWS\SYSTEM32\JavaMds.dll');
      end
      p=javaclasspath('-all');
      matches = strfind(p,'mdsobjects.jar');
      if not(any(vertcat(matches{:})))
        javaaddpath(strcat(MDSPLUS_DIR,'/java/classes/mdsobjects.jar'))
      end
      x=MDSplus.Data.execute('1',javaArray('MDSplus.Data',1));
    catch javaerror
      usePython=true;
    end
    if ~islogical(MDSplus_legacy_behavior)
      l = getenv('MDSplus_legacy_behavior');
      if strcmp(l,'yes')
        MDSplus_legacy_behavior = true;
      else
	MDSplus_legacy_behavior = false;
      end
    end
    if usePython
      try
        x=py.MDSplus.Int32(int32(1));
      catch pythonerror
        disp('Unable to connect to MDSplus using either a java bridge or a python bridge')
        disp(strcat(' Java error: ',javaerror.message))
        disp(strcat(' Python error: ',pythonerror.message))
        return
      end
    end
    info.isConnected=false;
    info.connection=[];
    info.connectedHost='';
    info.usePython=usePython;
    info.isPythonConnection=false;
    info.useLegacy=MDSplus_legacy_behavior;
    MDSINFO=info;
  end
  info=MDSINFO;
end
  
