/*	 CONCATENATE.FUN
; PURPOSE:	Given an arry of signals and an array of
;               start times, combine the signals into one
;               signal like the wavegen would produce.
; CATEGORY:	CMOD
; CALLING SEQUENCE: combined_sig = CONCATINATE(in_sigs, start_times)
; INPUTS:	in_sigs - array of nids of signals
;               start_times - array of start times for each element of in_sigs
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS: --
; OUTPUTS:	--
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS: --
; RESTRICTIONS: --
; PROCEDURE:	--
; MODIFICATION HISTORY:
;	JAS 16-MAR-1994 Initial coding.
;       JAS 17-MAR-2004 add optional min_delta argument (10 years +1 day later)
*/
FUN	PUBLIC CONCATENATE(_sigs, IN _starts, optional in _mindelta) {
  private _x_ans = [0.];
  private _y_ans = [0.];
  _mind = present (_mindelta) ? _mindelta : 1E-3;
  for (_i = 0; _i < SIZE(_sigs); _i++) {
     _sig = getnci(_sigs[_i], 'RECORD');
     if (_i == SIZE(_sigs)-1) {
       _x = [_starts[_i], pack(DIM_OF(_sig), dim_of(_sig) > _starts[_i])];
     } else {
       _x = [_starts[_i], pack(dim_of(_sig), (dim_of(_sig) > _starts[_i]) and (dim_of(_sig) < _starts[_i+1])), _starts[_i+1] - _mind];
     }
     _y = MdsMisc->Resample:DSC(XD(_SIG),XD(_x));
     if (_i == 0) {
       _x_ans = _x;
       _y_ans = _Y;
     } else {
         _x_ans = [_x_ans, _x];
         _y_ans = [_y_ans, _y];
     }
  }
  _x_ans = SET_RANGE(0 : SIZE(_X_ANS)-1, _X_ANS);
  _y_ans = SET_RANGE(0 : SIZE(_y_ANS)-1, _y_ANS);
/*  write(*, _x_ans); */
  return(MAKE_SIGNAL(_Y_ANS, *, _X_ANS));
}    
