function info = mdsInfo(varargin)
% MDSINFO  used internally by other functions to retrieve configuration info
    global MDSINFO
    if ~isempty(MDSINFO)
        info = MDSINFO;
    else
        l = getenv('MDSplus_legacy_behavior');
        if strcmp(l,'yes')
            MDSplus_legacy_behavior = true;
        else
            MDSplus_legacy_behavior = false;
        end
        info.isConnected = false;
        info.connection = [];
        info.connectedHost = '';
        info.usePython = false;
        info.isPythonConnection = false;
        info.useLegacy = MDSplus_legacy_behavior;
        MDSINFO = info;
    end
    if nargin > 0
        MDSINFO.usePython = varargin{1};
    end
    err = [true,true];
    if MDSINFO.usePython
        err(1) = setPython;
        if ~islogical(err(1))
            MDSINFO.usePython = false;
            err(2) = setJavaMds;
        end
    else
        err(2) = setJavaMds;
        if ~islogical(err(2))
            MDSINFO.usePython = true;
            err(1) = setPython;
        end
    end
    if any(~islogical(err))
        if all(~islogical(err))
            disp('Unable to connect to MDSplus using either a java bridge or a python bridge')
            disp(strcat(' Java error: ',err(2).message))
            disp(strcat(' Python error: ',err(1).message))
            info = [];
            MDSINFO = info;
        elseif ~islogical(err(2))
            disp('Unable to connect to MDSplus using java bridge, using python bridge instead')
            disp(strcat(' Java error: ',err(2).message))
        else % ~islogical(err(1))
            disp('Unable to connect to MDSplus using python bridge, using python bridge instead')
            disp(strcat(' Python error: ',err(1).message))
        end
    end
end

function err = setPython()
    persistent cache
    if ~isempty(cache)
        err = cache;
    else
        try
            py.MDSplus.Int32(1);
            err = false;
            cache = err;
        catch err
        end
    end
end

function err = setJavaMds()
    persistent cache
    if ~isempty(cache)
        err = cache;
    else
        isOctave = (exist ('OCTAVE_VERSION', 'builtin') > 0);
        try
            MDSPLUS_DIR = getenv('MDSPLUS_DIR');
            if ismac
                lib = strcat(MDSPLUS_DIR,'/lib/libJavaMds.dylib');
            elseif isunix
                lib = strcat(MDSPLUS_DIR,'/lib/libJavaMds.so');
            elseif ispc
                lib = 'C:\WINDOWS\SYSTEM32\JavaMds.dll';
                if ~isfile(lib)
                    if strcmp(computer('arch'),'win64')
                        lib = fullfile(MDSPLUS_DIR,'bin64','JavaMds.dll');
                    else
                        lib = fullfile(MDSPLUS_DIR,'bin32','JavaMds.dll');
                    end
                end
            end
            if not(isOctave)
                java.lang.System.setProperty('JavaMdsLib',lib);
            end
            p=javaclasspath('-all');
            matches = strfind(p,'mdsobjects.jar');
            if not(any(vertcat(matches{:})))
               javaaddpath(fullfile(MDSPLUS_DIR,'java','classes','mdsobjects.jar'))
            end
            if isOctave
                javaObject('MDSplus.Int32',1);
            else
                MDSplus.Data.execute('1',javaArray('MDSplus.Data',1));
            end
            err = false;
            cache = err;
        catch err
        end
    end
end