/*
   D3DDISPATCH.FUN

   This TDI function eliminates the problem where a phase from 
   the last shot gets dispatched during the next shot cycle.

   If the shot parameter does not match the current shot number, 
   then no phase is dispatched.  If the shot numbers match, the
   phase is dispatched.

   The reuturn value of 1 indicates success.
*/
PUBLIC FUN D3DDISPATCH(IN _SHOT, IN _PHASE)
{
	_CURRENT_SHOT = CURRENT_SHOT("D3D");
	IF ( _SHOT == _CURRENT_SHOT ) {
		TCL("DISPATCH/COMMAND/SERVER=mdsdispatcher DISPATCH/PHASE "//_PHASE);
	} ELSE {
		TCL("DISPATCH/COMMAND/SERVER=mdsdispatcher TYPE Ignored "//_PHASE//" for shot "//_SHOT//" (current shot is "//_CURRENT_SHOT//").");
	}

	RETURN(1);
}
