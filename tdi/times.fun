/*	 TIMES.FUN
; PURPOSE:	Convert array of system times to ascii times
; CATEGORY:	CMOD
; CALLING SEQUENCE: times_ascii = TIMES(times_system)
; INPUTS:	array of system times
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS: --
; OUTPUTS:	--
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS: --
; RESTRICTIONS: --
; PROCEDURE:	--
; MODIFICATION HISTORY:
;	TWF 28-Apr-1993 Initial coding.
*/
FUN	PUBLIC TIMES(IN _SYSTIMES) {
        _SYS = QUADWORD_UNSIGNED(DATA(_SYSTIMES));
	_TIME = *;
	For (_N=0; _N < SIZE(_SYS); _N++) {
		_TIME = [_TIME, DATE_TIME(_SYS[_N])];
	}
	Return (_TIME);
}

