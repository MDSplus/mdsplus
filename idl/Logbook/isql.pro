;+
; NAME:		ISQL
; PURPOSE:	Do interactive SQL expression.
; CATEGORY:	MDSPLUS
; CALLING SEQUENCE:
;	count = ISQL(expression	;valid SQL operation
;	[,COUNT=count]$		;number of selected items
;	[,ERROR=error]$		;VMS error code
;	[,HEAD=head]$		;Output format for header 0,1,2
;	[,QUIET=quiet]$		;suppress printing of error message
;	[,STATUS=status]$	;VMS error code
;	[,TEXT=text]$		;return as variable, not printed
;	[,WIDTH=width])		;nominal width of display text, default=term
; INPUT PARAMETERS:       expression = character string with SQL operation
; OPTIONAL INPUT PARAMETERS: None.
; KEYWORDS:	See above.
; OUTPUTS:	None except for SELECT operations where the remaining
;		arg1,...argn variables receive the output.
;		They may not be expressions or constants.
; COMMON BLOCKS: None.
; SIDE EFFECTS:	SQL database opened or accessed.
;	Markers in expression (?) will prompt for input.
; RESTRICTIONS:	First operation must be "DECLARE SCHEMA FILENAME xx" or else
;		SQL$DATABASE will be used and you are limited to 12 arguments.
; PROCEDURE:	This dummy procedure will LINKIMAGE to the real function.
;       	Once done, this procedure will not be reinvoked.
; MODIFICATION HISTORY:
;	 Idea by T.W. Fredian, September 22, 1992
;-
FUNCTION isql, p1, COUNT=count, ERROR=error, HEAD=head, $
	QUIET=quiet, STATUS=status, TEXT=text, WIDTH=width
SQL
if n_elements(quiet) eq 0 then quiet = 0
if n_elements(head) eq 0 then head = 2
if n_elements(width) eq 0 then width = -1
IF (KEYWORD_SET(text))$
THEN RETURN, CALL_FUNCTION('ISQL', p1, COUNT=count, ERROR=error, HEAD=head, $
	QUIET=quiet, STATUS=status, TEXT=text, WIDTH=width)
RETURN, CALL_FUNCTION('ISQL', p1, COUNT=count, ERROR=error, HEAD=head, $
	QUIET=quiet, STATUS=status, WIDTH=width)

END
