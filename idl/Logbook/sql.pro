PRO	SQL
  if !version.os eq 'vms' or !version.os eq 'Win32' then begin
    entry = 'IDLSQL'
    file = 'IDLSQL'
  endif else if !version.os eq 'OSF' or !version.os eq 'sunos' then begin
    entry = 'IDLSQL'
    file = 'libIdlSql.so'
  endif else if (!version.os eq 'hp-ux') then begin
	entry = 'IDLSQL'
        file=''
        path = getenv('SHLIB_PATH')
        if (strlen(path) gt 0) then begin
          paths = str_sep(path, ':')
          i = 0
          while i lt n_elements(paths) and strlen(file) eq 0 do begin
            file = findfile(paths(i)+'/libIdlSql.sl', count=count)
            if (count gt 0) then file = file(0)
            i = i + 1
          end
        endif
        if strlen(file) eq 0 then $
          file = findfile('/urs/local/lib/libIdlSql.sl')
        if strlen(file) eq 0 then $
           message, 'Could not find libIdlSql.sl in SHLIB_PATH'
  endif else begin
    message, 'Platform '+!version.os+' not yet supported'
  endelse

  LINKIMAGE_ONCE,'DSQL',file,1,entry+'_DYNAMIC',	/KEYWORDS,MIN_ARGS=1
  LINKIMAGE_ONCE,'ISQL',file,1,entry+'_INTERACTIVE',/KEYWORDS,MIN_ARGS=1,MAX_ARGS=1
  LINKIMAGE_ONCE,'DSQL_LOGIN',file,1,entry+'_LOGIN',/KEYWORDS,MIN_ARGS=3,MAX_ARGS=3
  LINKIMAGE_ONCE,'DSQL_LOGOUT',file,1,entry+'_LOGOUT',/KEYWORDS,MIN_ARGS=0,MAX_ARGS=0
  LINKIMAGE_ONCE,'DSQL_SETDBTYPE',file,0,entry+'_SETDBTYPE',/KEYWORDS,MIN_ARGS=0,MAX_ARGS=0
END
