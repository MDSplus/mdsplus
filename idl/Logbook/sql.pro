PRO	SQL
  if !version.os eq 'vms' or !version.os eq 'Win32' then begin
    entry = 'IDLSQL'
    file = 'IDLSQL'
  endif else if !version.os eq 'OSF' then begin
    entry = 'IDLSQL'
    file = 'libIdlSql.so'
  endif else begin
	entry = '_IDLSQL'
	file = getenv('SYBASE')+'/idl/sql.so'
  endelse

  LINKIMAGE_ONCE,'DSQL',file,1,entry+'_DYNAMIC',	/KEYWORDS,MIN_ARGS=1
  LINKIMAGE_ONCE,'ISQL',file,1,entry+'_INTERACTIVE',/KEYWORDS,MIN_ARGS=1,MAX_ARGS=1
  LINKIMAGE_ONCE,'DSQL_LOGIN',file,1,entry+'_LOGIN',/KEYWORDS,MIN_ARGS=3,MAX_ARGS=3
  LINKIMAGE_ONCE,'DSQL_LOGOUT',file,1,entry+'_LOGOUT',/KEYWORDS,MIN_ARGS=0,MAX_ARGS=0
  LINKIMAGE_ONCE,'DSQL_SETDBTYPE',file,0,entry+'_SETDBTYPE',/KEYWORDS,MIN_ARGS=0,MAX_ARGS=0
END
