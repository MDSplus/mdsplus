/*	 OUTPUT_NODE.FUN
; PURPOSE:	given a node spec which may be a reference
;               to another node return the name of the primative
;               node at the end of the chain.
; CATEGORY:	PCS
; CALLING SEQUENCE: path = OUTPUT_NAME(node_spec)
; INPUTS:	node_spec - name of the node to get.
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
*/
FUN	PUBLIC OUTPUT_NAME(IN _node) {
  write(*, _node);
  if (GETNCI(_node, 'NID_REFERENCE')) {
    write(*, 'It is a nid');
    _tmp = GETNCI(_node, 'RECORD');
    write(*, 'Got the record');
    _nid = GETNCI(_tmp, 'NID_NUMBER');
    write(*, 'Got the nid number');
    return (OUTPUT_NAME(_NID ) );
  }  else {
     write (*, 'Unwinding  answer is:');
     _ans = GETNCI(_node, "FULLPATH");
     write(*, _ans);
     return (_ans);
   }
}
