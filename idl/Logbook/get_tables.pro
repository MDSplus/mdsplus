FUNCTION GET_TABLES, tables, rows, flags, ALL=all, MATCH=match, SYSTEM=system, QUIET=quiet, STATUS=status
;+
; NAME:		GET_TABLES
; PURPOSE:	Get user/system/all Rdb tables
; CATEGORY:	MDSplus
; CALLING SEQUENCE: rows = GET_TABLES(
; INPUTS:	--
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS:
;		/ALL to all tables, default is the user tables
;		/SYSTEM to get system tables
;		MATCH=match a string to be matched using wildcards.
;		Use % for any string, _ for any one character.
;		/QUIET to suppress error messages.
;		STATUS=status to get error code.
; OUTPUTS:	--
; OPTIONAL OUTPUT PARAMETERS:
;		TABLES is trailing blank string name(s).
;		ROWS is the number of rows in each table. Not avail. for Sybase.
;		FLAGS is 1 for a view.
; COMMON BLOCKS: --
; SIDE EFFECTS:	Opens SQL$DATABASE if no database defined.
; RESTRICTIONS:	VAX dynamic SQL.
;		Requires declared database.
;		Rdb only.
; PROCEDURE:	Query the relations table.
; MODIFICATION HISTORY:
;	KKlare, LANL P-4 (c)06-May-1991 Based on SHOW_TABLES
;	KKlare, LANL P-4 (c)09-May-1991 Add table select
;	KKlare, LANL P-4 (c)26-Aug-1991 Add status
;	KKlare, LANL P-4 (c)22-Jul-1993 Add Sybase
;-

if n_elements(quiet) eq 0 then quiet = 0

IF get_database() EQ 'RDB' THEN BEGIN
	z = "SELECT Rdb$relation_name, Rdb$cardinality, Rdb$flags FROM Rdb$relations WHERE Rdb$system_flag <> "
	IF	KEYWORD_SET(all) 	THEN	z=z+'2' $
	ELSE IF	KEYWORD_SET(system)	THEN	z=z+'0' $
	ELSE					z=z+'1'
	IF N_ELEMENTS(match) GT 0 THEN z=z+" AND Rdb$relation_name LIKE '"+STRUPCASE(match)+" %'"
	z = z+" ORDER BY Rdb$relation_name"
	n = DSQL(z, tables, rows, flags, QUIET=quiet, STATUS=status)
	z = DSQL('Rollback')
ENDIF ELSE BEGIN
	z = "SELECT name, -1, type FROM sysobjects WHERE type IN ("
	IF	KEYWORD_SET(all)	THEN	z=z+"'S ','U ','V ')"	$
	ELSE IF KEYWORD_SET(system)	THEN	z=z+"'S ')"		$
	ELSE					z=z+"'U ','V ')"
	IF N_ELEMENTS(match) GT 0 THEN z=z+" AND name LIKE '"+match+"%'"
	IF STRPOS(z, "% %'") GE 0 THEN z = STRMID(z, 0, STRLEN(z)-3) + "'"
	z = z+" ORDER BY name"
	n = DSQL(z, tables, rows, flags, QUIET=quiet, STATUS=status)
	flags = STRING(flags) EQ 'V '
ENDELSE
RETURN, n
END
