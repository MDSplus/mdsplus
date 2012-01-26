fun public mdsversion()
/*	mdsversion()

	Get string with version number of libTdiShrExt.so

call:   mdsversion()

	BPDuval: [CRPP, EPFL, Switzerland]	August 2000
*/
{
   return(TdiShrExt->rMdsVersion:dsc());
}
