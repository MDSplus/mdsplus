function err = javaActivate()
persistent cache
if ~isempty(cache)
    err = cache;
else
    isOctave = (exist ('OCTAVE_VERSION', 'builtin') > 0);
    try
        MDSPLUS_DIR = getenv('MDSPLUS_DIR');
        if ismac
            lib = strcat(MDSPLUS_DIR, '/lib/libJavaMds.dylib');
        elseif isunix
            lib = strcat(MDSPLUS_DIR, '/lib/libJavaMds.so');
        elseif ispc
            lib = 'C:\WINDOWS\SYSTEM32\JavaMds.dll';
            if ~isfile(lib)
                if strcmp(computer('arch'), 'win64')
                    lib = fullfile(MDSPLUS_DIR, 'bin64', 'JavaMds.dll');
                else
                    lib = fullfile(MDSPLUS_DIR, 'bin32', 'JavaMds.dll');
                end
            end
        end
        if not(isOctave)
            java.lang.System.setProperty('JavaMdsLib', lib);
        end
        p = javaclasspath('-all');
        matches = strfind(p, 'mdsobjects.jar');
        if not(any(vertcat(matches{:})))
            javaaddpath(fullfile(MDSPLUS_DIR, 'java', 'classes', 'mdsobjects.jar'))
        end
        if isOctave
            javaObject('MDSplus.Int32', 1);
        else
            MDSplus.Data.execute('1', javaArray('MDSplus.Data', 1));
        end
        err = false;
        cache = err;
    catch err
    end
end