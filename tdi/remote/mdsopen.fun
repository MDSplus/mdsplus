fun public mdsopen(optional in _host, optional in _shot)
/*	mdsopen(optional in _host, optional in _shot)

	Open a remote tdi session using mdsip

call:   
	_host		= Host name eg elpp1:: or elpp1::tcv_shot:9000
	_shot		= shot 

	BPDuval: [CRPP, EPFL, Switzerland]	March 2000
*/
{
/* first get the connection to open */
   if(mdscurrent() == 'local') {
      return(treeopen(_host,_shot));
   }
   if(!present(_host))  {
      _host=TranslateLogical("MDS_HOST")//"::";
      write(*,"Host taken from MDS_HOST ["//_host//"]");
   }
/* open the shot */
   if(!present(_shot))  {
      _status = build_call(8,'TdiShrExt','rMdsOpen',_host,val(0l));
   } else {
      _status = build_call(8,'TdiShrExt','rMdsOpen',_host,long(_shot));
   }
   return(_status);
}
