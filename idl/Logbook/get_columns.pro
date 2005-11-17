
FUNCTION GET_COLUMNS, table, columns, types, lengths, scales, tables, MATCH=match, QUIET=quiet, STATUS=status, SORTED=sorted, CASE_INSENSITIVE=case_insensitive
;+
; NAME:		GET_COLUMNS.PRO
; PURPOSE:	Get column names of an Rdb table.
; CATEGORY:	??
; CALLING SEQUENCE:
;	rows = GET_COLUMNS('table'[,columns[,types[,lengths[,scales[,tables]]]]])
; INPUTS:	TABLE is the name of a table, can be wildcarded.
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS:
;	MATCH=match a string to be matched using wildcards.
;		Use % for any string, _ for any one character.
;		Sybase is case-sensitive.
;       /SORTED - if present sort the output alphabetically
;                 /CASE_INSENSITIVE - if present, and /SORTED is
;                                     present sort is done case insensitively.
;	/QUIET to suppress error messages.
;	STATUS=status to get error code.
; OPTIONAL OUTPUT PARAMETERS:
;		COLUMNS is the column names.
;		TYPES is the VMS data type of Rdb or Sybase type number.
;type/VMS Fortran	C	IDL	SQL			Sybase
;6 B	byte		char	--	514 TINYINT		48 SYBINT1
;7 W	integer*2	word	integer	500 SMALLINT		52 SYBINT2
;8 L	integer*4	long	long	496 INTEGER		56 SYBINT4
;9 Q	--		--		504 QUADWORD		--
;10 F	real*4		float	float	480 REAL FLOAT(len=4)	59 SYBREAL
;14 T	character	char*	string	452 CHAR		47 SYBCHAR
;27 G	real*8		double	double	480 DOUBLE PRECISION	62 SYBFLT8
;35 ADT	--		--	--	502 DATE		61 SYBDATETIME/111 SYBDATETIMN
;37 VT	--		--	string	448 VARCHAR		23 SYBVARCHAR
;261 --	--		--	--	508 SEGMENTED STRING ID	35 SYBTEXT/34 SYBIMAGE
;		LENGTHS is the number of bytes per element.
;		SCALES is the places after the decimal (integers).
;		TABLES is the name of the table found.
; SIDE EFFECTS:	Opens VMS SQL$DATABASE if no database defined.
; RESTRICTIONS:	VAX dynamic SQL or Sun-Sybase
;		Requires declared database
;		VMS descriptor types.
; PROCEDURE:	Join Rdb/Sybase tables to get info.
; MODIFICATION HISTORY:
;	KKlare, LANL P-4 (c)07-May-1991 Initial coding
;	KKlare, LANL P-4 (c)09-May-1991 Allow selection
;	KKlare, LANL P-4 (c)26-Aug-1991 Add status, remove domains, add tables
;	KKlare, LANL P-4 (c)22-Jul-1993 Add Sybase
;-
IF n_elements(quiet) eq 0 then quiet = 0
	z = "SELECT o.name, c.name, c.type, c.length, 0"	$
	+" FROM syscolumns c, sysobjects o"			$
	+" WHERE o.name = '" + table	+ "' AND o.id = c.id"
	IF N_ELEMENTS(match) GT 0 THEN z = z+$
		" AND c.name LIKE '" + match + "%'"
	z = z + " ORDER BY o.name, ABS(c.offset)"
	n = DSQL(z, tables, columns, types, lengths, scales,$
		QUIET=quiet, STATUS=status)
        if (keyword_set(sorted)) ne 0 then $
          if (keyword_set(case_insensitive)) then $
            columns = columns(sort(strupcase(columns))) $
          else $
            columns = columns(sort(columns))

RETURN, n
END
