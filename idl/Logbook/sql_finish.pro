PRO	SQL_FINISH, schema, QUIET=quiet, STATUS=status
;+
; NAME:	 SQL_FINISH.PRO
; PURPOSE:	Detach from all schema.
; CATEGORY:	MDSplus
; CALLING SEQUENCE: SQL_FINISH,'filename'
; INPUTS:	schema is pathname of database.
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS:
;		/QUIET to suppress error messages.
;		STATUS=status to get error code.
; OUTPUTS:	--
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS: Does COMMIT of changed databases.
; RESTRICTIONS: Uses VAX dynamic SQL.
; PROCEDURE:	--
; MODIFICATION HISTORY:
;	Ken Klare, LANL P-4 (c)10-May-1991	Initial coding.
;	Ken Klare, LANL P-4 (c)26-Aug-1991	Add status
;	Ken Klare, LANL P-4 (c)01-Jun-1993	Sybase
;	Ken Klare, LANL P-4 (c)23-Jul-1993	Environment variable
;-
status = 0L
typ = get_database()
if (typ eq 'SYBASE') then $
   dummy = DSQL_LOGOUT() $
else if  (typ eq 'RDB') then $
   dummy = DSQL('DISCONNECT ALL')
defsysv, '!DATABASE', exists=exists
if (exists) then $
  dummy = execute("!DATABASE.DBTYPE = ''")
END
