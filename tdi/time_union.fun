/*	 TIME_UNION.FUN
; PURPOSE:	Get union of dimensions of nodes which are on
;               and match the wildcard spec.
; CATEGORY:	CMOD
; CALLING SEQUENCE: times = TIME_UNION(wild-card-node-spec)
; INPUTS:	string containing wild card node specification
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS: --
; OUTPUTS:	--
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS: --
; RESTRICTIONS: --
; PROCEDURE:	--
; MODIFICATION HISTORY:
;	TWF 30-Aug-1991 Initial coding.
*/
FUN	PUBLIC TIME_UNION(IN _PATH) {
	_NIDS = GETNCI(_PATH, "NID_NUMBER");
	_NIDS = PACK(_NIDS, GETNCI(_NIDS,"ON"));
	_DIM = *;
	For (_N = SIZE(_NIDS); --_N >= 0; *) {
		_DIM = [_DIM, DIM_OF(GETNCI(_NIDS[_N], "RECORD"))];
	}
        if (size(_dim) > 0) {
  	  Return (UNION(_DIM));
        } else {
	  Return (0);
	}
}

