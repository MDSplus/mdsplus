/*       NODEREFS_OF.FUN
; PURPOSE:      Returns a string array listing the node
;               references found in an expression.
;
;               Recursively examines expressions.
;
; CATEGORY:     MDSPLUS
;
; CALLING SEQUENCE: _noderefs = PHASE_NAME_LOOKUP(_input)
;
; INPUTS:       _input = a STRING containing the expression
;                        to examine
;
; OUTPUTS:     _noderefs = a string array with the 
;                          node references found.  Will be
;                          ROPRAND if none are found.
;
; EXAMPLES:    _noderefs = NODEREFS_OF("\TOP:PINJ/\PINJ_15L");
;
; MODIFICATION HISTORY:
;       Jeff Schachter 2001.03.29 - initial coding
*/

PUBLIC FUN NODEREFS_OF(IN _input)
{

  _k = EXECUTE("KIND_OF("//_input//")");
  if (_k == 192 || _k == 193) {return(_input);}

  private _value = [];

  _c = EXECUTE("CLASS("//_input//")");

  if (_c == 194) 
  {
     _n = EXECUTE("NDESC_OF("//_input//")");

       for (_i=0; _i<_n; _i++)
       {
         _cmd = "DECOMPILE(`DSCPTR_OF("//_input//","//_i//"))";
         _piece = EXECUTE(_cmd);
         _value = [_value, NODEREFS_OF(_piece)];
       }

  }

  RETURN(_value);

}
	
