/*	 CROP_LEFT.FUN
; PURPOSE:	Get union of dimensions of nodes which are on
;               and match the wildcard spec.
; CATEGORY:	CMOD
; CALLING SEQUENCE: sig = CROP_LEFT(signal, start_time)
; INPUTS:	_signal - a signal to be operated on
;               _start_time - time of new first point in output signal
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS: --
; OUTPUTS:	returns a signal chopped off on the left.  The returned
;               signal does not have a raw component.
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS: --
; RESTRICTIONS:	sig is a one d signal with sorted dimensions
; PROCEDURE:	--
; MODIFICATION HISTORY:
;	JAS 15-SEP-1995 Initial coding.
*/
FUN	PUBLIC CROP_LEFT(IN _SIG, IN _START) {
        _DIM = DIM_OF(_SIG);
        if (_start == _dim[0]) return(_sig);
        _DAT = DATA(_SIG);
        if (_START < _DIM[0]) return(make_signal([_dat[0],_dat], *, [_start,_dim]));
        _last = size(_dim) -1;
        if (_START >= _DIM[_last]) return (make_signal([_dat[_last], _dat[_last]],*, [_START,_START+10.]));
        _idx = _dim >= _start;
        _prev = shape(_dim)[0] - sum(_idx) - 1;
        _prev_tim = _dim[_prev];
        _prev_v = _dat[_prev]; 
        _dim = pack(_dim,_idx); 
        _dat = pack(_dat, _idx);
        if (_start == _dim[0]) return(make_signal(_dat,*, _dim));
        _nxt_time = _dim[0];
        _nxt_val = _dat[0];
        _start_val = _nxt_val - (_nxt_time - _start)*(_nxt_val - _prev_v)/(_nxt_time - _prev_tim);
        Return(make_signal([_start_val,_dat],*, [_start, _dim]));
}
