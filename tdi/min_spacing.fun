/*	 MIN_SPACING.FUN
; PURPOSE:	Transform a sorted  input array of numbers
;               so that there is at least _SPACE between them.
;               - If not then move offenders so that there is.
; CATEGORY:	CMOD
; CALLING SEQUENCE: new_array = MIN_SPACEING(src_array, space)
; INPUTS:	sorted array of inputs,
;               desired minimum space between the values.
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS: --
; OUTPUTS:	--
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS: --
; RESTRICTIONS: --
; PROCEDURE:	--
; MODIFICATION HISTORY:
;	JAS 26-AUG-1992 Initial coding.
*/
FUN	PUBLIC MIN_SPACING(IN _ARR_IN, IN _SPACE) {
  FOR ( _BADSEGS = [0, _ARR_IN[1 : *] - _ARR_IN < _SPACE] ; 
       SUM(_BADSEGS)>0; 
       _BADSEGS = [0, _ARR_IN[1 : *] - _ARR_IN < _SPACE]) {
    _FIXED = [_ARR_IN[0], _ARR_IN + _SPACE];
    _ARR_IN = MERGE(_FIXED, _ARR_IN, _BADSEGS);
  };
  RETURN(_ARR_IN);
}
