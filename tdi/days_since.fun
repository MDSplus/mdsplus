/*      DAYS_SINCE.FUN
; PURPOSE:      Return a floating point number of days
;               given an input quadword time.
; CATEGORY:     Data Conversion
; CALLING SEQUENCE: days = DAYS_SINCE(quadword-time [,_since-time-string])
; INPUTS:
;               quadword-time - a VMS time or array of times.
;
; OPTIONAL INPUT PARAMETERS: --
;               _since-time-string   - date to start from
;                                      default '1/1/92'
;               format must be:
;                  'today'
;		   'tomorrow'
;                  'yesterday'
;                  'mm/dd/yy [hh:mm]'
;
; OUTPUTS:      --
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS: --
; RESTRICTIONS: --
; PROCEDURE:    --
; MODIFICATION HISTORY:
;       JAS 14-OCT-1992 Initial coding.
*/
FUN     PUBLIC DAYS_SINCE(IN _time, OPTIONAL IN _since) {
  _DELTA = 0Q;
  _ANS = 0.0;
  if (not PRESENT(_since)) _since = '1/1/92';
  _stat = MdsShr->LibConvertDateString( _since,REF(_DELTA));
  _q_times = _time - _delta;
  _q_times = _q_times / 100000Q;  /* hundredths of a second */
  _ans = FLOAT(_q_times)/(24.*360000.);
  return(_ans);
}
