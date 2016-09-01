/*      SUBMIT.FUN
; PURPOSE:      Submit a batch job.
; CATEGORY:     Scheduling
; CALLING SEQUENCE: days = SUBMIT(shell-script) 
; INPUTS:
;               shell-script - the name of a command procedure to 
;                                  submit.
;
; PROCEDURE:  uses unix_submit.sh to create an analysis job with the currently open shot number as $1 and
;             the stdout and sterr added to the end of the file /var/mdsplus/log/{base-name}.log
; EXAMPLES:
;      PRINT, MDS$VALUE("SUBMIT('/usr/local/cmod/codes/my_anal/my_anal.sh')")
;
; MODIFICATION HISTORY:
;       JAS  8-APR-2002 Initial coding.
*/

public fun unix_submit(in _file)
{
  TCL("spawn/nowait unix_submit "//_file//" "//$SHOT);
}
