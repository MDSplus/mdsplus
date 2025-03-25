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
   _invalid_id = -1;
   if (present(_abort) && _status == _invalid_id) abort();
   return(_status);
}
