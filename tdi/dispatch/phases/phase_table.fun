/*	 PHASE_TABLE.FUN
; PURPOSE:	Site customizable phase table
;               for use with MIT Dispatch/Serve
; CATEGORY:	CMOD (site customizable)
; CALLING SEQUENCE: phase_table = PHASE_TABLE()
; INPUTS:	 --
; OPTIONAL INPUT PARAMETERS: --
; KEYWORD PARAMETERS: --
; OUTPUTS:	--
; OPTIONAL OUTPUT PARAMETERS: --
; COMMON BLOCKS: --
; SIDE EFFECTS: --
; RESTRICTIONS: --
; PROCEDURE:	--
; MODIFICATION HISTORY:
;	TWF 3-AUG-1992 Initial coding.

This function should return a phase table specific to a MDSplus site. The
phase table is a one dimensional string array containing a string pair for
each defined phase. The string pair consists of the phase name and value,
for example: 'PHASE_ONE','1'.

*/
FUN	PUBLIC PHASE_TABLE() {
          RETURN ( [
                     'OFF',                         '0',
                     'INITIALIZATION',              '5',
		     'INIT',			    '5',
                     'PULSE_ON',                    '6',
                     'PON',                         '6',
                     'BULK_DATA_ACQUISITION',       '9',
                     'STORE',                       '9',
                     'SECONDARY_DATA_ACQUISITION', '10',
                     'ANALYSIS',                   '10',
                     'DEINIT',                     '90',
		     'FINISH_SHOT',                '99'
                  ]);
}
