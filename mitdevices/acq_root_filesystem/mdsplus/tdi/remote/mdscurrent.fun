fun public mdscurrent()
/*	mdscurrent()

	Get string with current connection name

call:   mdscurrent()

	BPDuval: [CRPP, EPFL, Switzerland]	August 2000
*/
{
   return(TdiShrExt->rMdsCurrent:dsc() );
}
