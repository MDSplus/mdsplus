;+
; NAME:		DSQL
; PURPOSE:	Do dynamic SQL expression.
; CATEGORY:	MDSPLUS
; CALLING SEQUENCE:
;	ans = DSQL(expression [,arg1,...,argn]
;	[,COUNT=count]$		;number of selected items
;	[,DATE=date]$		;binary date format
;	[,ERROR=error]$		;VMS error code
;	[,EXCESS=excess]$	;allow excessive args (ignored on this pass)
;	[,QUIET=quiet]$		;suppress printing of error message
;	[,STATUS=status])	;VMS error code
; INPUT PARAMETERS:       expression = character string with SQL operation
; OPTIONAL INPUT PARAMETERS:
;	arg1,...,argn = values to substitute into the expression for
;	"?" input parameters.
; KEYWORDS:	See above.
; OUTPUTS:	None except for SELECT operations where the remaining
;		arg1,...argn variables receive the output.
;		They may not be expressions or constants.
; COMMON BLOCKS: None.
; SIDE EFFECTS:	SQL database opened or accessed.
; RESTRICTIONS:	First operation must be "DECLARE SCHEMA FILENAME xx" or else
;		SQL$DATABASE will be used and you are limited to 12 arguments.
; PROCEDURE:	This dummy procedure will LINKIMAGE to the real function.
;       	Once done, this procedure will not be reinvoked.
; MODIFICATION HISTORY:
;	 Idea by T.W. Fredian, September 22, 1992
;-
FUNCTION dsql,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,COUNT=count,$
	DATE=date,ERROR=error,EXCESS=excess,QUIET=quiet,STATUS=status,host=host
SQL
if n_elements(quiet) eq 0 then quiet=0
if n_elements(date) eq 0 then date=0
if (n_elements(host) gt 0) then $
  RETURN, CALL_FUNCTION('DSQL',p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,$
	COUNT=count,DATE=date,ERROR=error,/EXCESS,QUIET=quiet,STATUS=status,HOST=host) $
else $
  RETURN, CALL_FUNCTION('DSQL',p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,$
	COUNT=count,DATE=date,ERROR=error,/EXCESS,QUIET=quiet,STATUS=status)
END
