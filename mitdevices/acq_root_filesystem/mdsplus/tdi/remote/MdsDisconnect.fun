fun public mdsdisconnect(optional in _all)
/*	mdsdisconnect(optional in _all)

	Disconnect given socket from session

call:   

	mdsdisconnect(optional in _all)
	if _all included and non-zero then all open connections will be closed.

	BPDuval: [CRPP, EPFL, Switzerland]	March 2000
*/
{
   if (present(_all))
     return(TdiShrExt->rMdsDisconnect(val(_all)));
   else
     return(TdiShrExt->rMdsDisconnect(val(0)));
}
