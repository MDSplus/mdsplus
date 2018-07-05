function info = mdsInfo()
%mdsInfo - used internally by other functions to retrieve configuration info
%   
%
 global MDSINFO
  global MDSplus_legacy_behavior
  if isempty(MDSINFO)
    isOctave = (exist ('OCTAVE_VERSION', 'builtin') > 0);
    usePython=false;
    try
      MDSPLUS_DIR=getenv('MDSPLUS_DIR');
      if ismac
        lib=strcat(MDSPLUS_DIR,'/lib/libJavaMds.dylib');
      elseif isunix
        lib=strcat(MDSPLUS_DIR,'/lib/libJavaMds.so');
      elseif ispc
        lib='C:\WINDOWS\SYSTEM32\JavaMds.dll';
      end
      if not(isOctave)
        java.lang.System.setProperty('JavaMdsLib',lib);
      end
      p=javaclasspath('-all');
      matches = strfind(p,'mdsobjects.jar');
      if not(any(vertcat(matches{:})))
        javaaddpath(strcat(MDSPLUS_DIR,'/java/classes/mdsobjects.jar'))
      end
      if isOctave
        x=javaObject('MDSplus.Int32',1);
      else
        x=MDSplus.Data.execute('1',javaArray('MDSplus.Data',1));
      end
    catch javaerror
      if isOctave
        disp('Unable to connect to MDSplus using the java bridge')
	return
      end
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
        py_MDSplus_Int32=str2func('py.MDSplus.Int32');
        x=py_MDSplus_Int32(int32(1));
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
  
