/*	 LOAD_PHASES.FUN
; PURPOSE:	Load phase name/value arrays in public variables
;               for use with MIT Dispatch/Serve
; CATEGORY:	CMOD (site customizable)
; CALLING SEQUENCE: phase_number = LOAD_PHASES()
; INPUTS:	 none
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS: --
; OUTPUTS:	--
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS: --
; RESTRICTIONS: --
; PROCEDURE:	--
; MODIFICATION HISTORY:
;	TWF 25-OCT-1993 Initial coding.
*/
FUN PUBLIC LOAD_PHASES() {
  _PHASES = PHASE_TABLE();
  _PHASE_NAMES = *;
  _PHASE_VALUES = *;
  FOR (_i=0;_i<size(_phases);) {
    _PHASE_NAMES = [_PHASE_NAMES, _PHASES[_i++]];
    _PHASE_VALUES = [_PHASE_VALUES,LONG(COMPILE(_PHASES[_i++]))];
  }
  _SORTI = SORT(_PHASE_NAMES);
  PUBLIC _$$$PHASE_VALUES = _PHASE_VALUES[_SORTI];
  PUBLIC _$$$PHASE_NAMES = _PHASE_NAMES[_SORTI];
  RETURN();
}
