/*	 EVENT_LOOKUP.FUN
; PURPOSE:	Return event numbers of CMOD events
;               for use with encoders and decoders.
; CATEGORY:	CMOD
; CALLING SEQUENCE: event_number = EVENT_LOOKUP(string | string-array)
; INPUTS:	string or string-array of known CMOD events.
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS: --
; OUTPUTS:	--
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS: --
; RESTRICTIONS: --
; PROCEDURE:	--
; MODIFICATION HISTORY:
;	TWF 23-SEP-1991 Initial coding.
*/
FUN	PUBLIC EVENT_LOOKUP(IN _IN_EVENT) {
        _EVENTS = EVENT_TABLE();
        _EVENT_NAMES = _EVENTS[0:254:2];
        _EVENT_VALUES = _EVENTS[1:255:2];
        _EVENT = UPCASE(_IN_EVENT);
        _NUM = *;
        _SORTI = SORT(_EVENT_NAMES);
        For (_N = 0; _N < SIZE(_EVENT); _N++) {
          _ENUM = BSEARCH(_EVENT[_N],_EVENT_NAMES[_SORTI],0);
          IF (_ENUM >= 0) {
              _NUM = [_NUM, COMPILE(_EVENT_VALUES[_SORTI[_ENUM]])];
          }
          else
          {
            WRITE(*,"Unknown event name specified /",_EVENT[_N],"/");
            ABORT();
          }
        }
        Return (UNION(_NUM));
}
