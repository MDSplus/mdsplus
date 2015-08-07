public fun mdsconnect(in _host, optional in _abort)
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
   _status = TdiShrExt->rMdsConnect(ref(_host//char(0)));
   if (present(_abort) && _status == 0) abort();
   return(_status);
}
