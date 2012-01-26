fun public mdsremotelist()
/*	mdsremotelist()

	List the current connection with mdsip

call:   mdsremotelist()

	BPDuval: [CRPP, EPFL, Switzerland]	August 2000
*/
{
   return(TdiShrExt->rMdsList());
}
