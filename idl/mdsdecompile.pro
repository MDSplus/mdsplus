Function MDSDECOMPILE,node_name,QUIET=QUIET,STATUS=STATUS,INFO=INFO
;
;+
; NAME:
;	MDSDECOMPILE
; PURPOSE:
;	Returns the decompilation of a node specified by name
; CATEGORY:
;	MDSPLUS
; CALLING SEQUENCE:
;	answer = MDSDECOMPILE(node-spec[,/QUIET],[STATUS=status][,/INFO])
; INPUT PARAMETERS:
;	node_spec = name of the node to decompile
; Keywords:
;       QUIET = prevents IDL error if TCL command fails
;       STATUS = return status, low bit set = success
;	INFO - if this keyword is set the VMS error will be displayed but
;              control will be returned to the caller instead of doing a
;              longjump back to the IDL prompt.
; OUTPUTS:
;       istat = return status, low bit set = success
;	answer = the decompiled text result of function (string).
; COMMON BLOCKS:
;	None.
; SIDE EFFECTS:
;	None.
; RESTRICTIONS:
;	None.
; PROCEDURE:
;	written to avoid the evaluate / don't evaluate
;       delemma for MDS$VALUE calls.
;	make a string to hand to MDS$VALUE.
; MODIFICATION HISTORY:
;	 VERSION 1.0, CREATED BY Josh Stillerman, June 25,1991
;	 VERSION 1.1, Removed EXECUTE  Steve Wolfe June 26, 1991
;	 VERSION 1.2, Fixed '\' vs '\\' problem.  Josh Stillerman Sept. 24, 1992
;-
;
nid=1L
IF NOT KEYWORD_SET(INFO) THEN INFO = 0
nid = mdsvalue('_rec=getnci($,"RECORD"),1',node_name,status=status,/quiet)
IF NOT STATUS AND NOT KEYWORD_SET(QUIET) THEN message,mdsgetmsg(status),INFO=INFO
IF STATUS THEN BEGIN
  cmd = 'decompile(`_rec)'
  IF NOT KEYWORD_SET(QUIET) THEN QUIET = 0
  RETURN,mdsvalue(cmd,QUIET=QUIET,STATUS=STATUS)
ENDIF ELSE RETURN,''
END
