fun public mdsclose()
/*	mdsclose()

	Close any open shots

call:   mdsclose()

	BPDuval: [CRPP, EPFL, Switzerland]	March 2000
*/
{
   if(mdscurrent() == 'local')
     return(treeclose());
   else
     return(TdiShrExt->rMdsClose());
}
