public fun mdsconnect(in _host)
/* mdsconnect(in _host)

Open a remote tdi session using mdsip

call:	_host  = Host name eg elpp1.epfl.ch or elpp1.epfl.ch:9000

	TWFredian: [MIT, USA] August 2000
*/
{
   if(!present(_host))  {
      _host=TranslateLogical("MDS_HOST")//"::";
      write(*,"Host taken from MDS_HOST ["//_host//"]");
   }
   _i = index(_host,":");
   if(_i == -1)
     _name = _host//"::";
   else
     _name = extract(0,_i,_host)//"::"//extract(_i,len(_host)-_i,_host);
/*   return(mdsopen((_i == -1) ? _host//"::" :extract(0,_i,_host)//"::"//extract(_i,len(_host)-_i,_host)));*/
   return(build_call(8,'TdiShrExt','rMdsOpen',_name,val(0l)));
}
