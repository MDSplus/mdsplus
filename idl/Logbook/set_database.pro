PRO	SET_DATABASE, database, QUIET=quiet, STATUS=status
;+
; NAME:		SET_DATABASE.PRO
; PURPOSE:	Declare schema for SQL.
; CATEGORY:	MDSplus
; CALLING SEQUENCE: SET_DATABASE, schema[, QUIET=quiet][, STATUS=status]
; INPUTS:	schema is a logical or physical name.
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS: --
;		/QUIET to suppress error messages.
;		STATUS=status to get error code.
; OUTPUTS:	--
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS:	Use COMMIT or ROLLBACK to change databases.
; RESTRICTIONS:	Uses VAX dynamic SQL.
;		To create/maintain a database you must be licensed
;		for Development or Interactive, not Runtime.
;		Must issue SQL command before to define linkimage.
; PROCEDURE:
; MODIFICATION HISTORY:
;	Ken Klare, LANL P-4 (c)7-May-1991	Initial coding.
;	Ken Klare, LANL P-4 (c)26-Aug-1991	Add status
;	Tom Fredian, MIT	connect avoids detaching and attaching each time
;       Josh Stillerman, MIT    rewritten to use dbinfo
;-
  SQL
  if n_elements(quiet) eq 0 then quiet = 0
  status = dbinfo(database, host, conn, dbtype, dbuser, dbpass)
  if (not status) then begin
    if (not quiet) then begin
      Message, "Error getting connection information for database "+database
    endif
    return
  endif

  if (host eq "") then begin
    Message, "The environment variable SYBASE_HOST must contain the name of your SQLSERVER"
    return
  endif

  db = get_database()
  if (strlen(db) gt 0) then begin
    dummy = execute('stat = !database.name eq database')
    if (stat) then begin
      return
    endif

    dummy = execute(' stat= (host ne !database.host) or (dbuser ne !database.user)')
    if stat then $
      sql_finish
  endif

  db = get_database()
  if (strlen(db) eq 0) then begin
    if (dbtype eq 'SYBASE') then $
      call_procedure,'DSQL_SETDBTYPE',/SYBASE $
    else if dbtype eq 'RDB' then begin
      call_procedure,'DSQL_SETDBTYPE',/RDB
    endif else begin
      if (not quiet) then $
        Message, "Invalid database type "+dbtype
      return
    endelse

    if (dbtype eq 'SYBASE') then begin
      status=CALL_FUNCTION('DSQL_LOGIN', host, dbuser, dbpass)
      if (not status) then begin
        if (not quiet) then $
          Message, "Could not connect to database"
        return
      endif
    endif
  endif


  for i=0, n_elements(conn)-1 do begin
    dummy = call_function('dsql', conn(i))
  endfor

  defsysv, '!database', exists=exists
  if (not exists) then $
    defsysv, '!database', {host:host, name:database, user:dbuser, dbtype:dbtype} $
  else begin
    dummy = execute('!database.host = host')
    dummy = execute('!database.name=database')
    dummy = execute('!database.user=dbuser')
    dummy = execute('!database.dbtype=dbtype')
  endelse
end
