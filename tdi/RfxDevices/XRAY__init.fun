public fun XRAY__init(as_is _nid, optional _method)
{

write(*, "XRAY init");

      if( present(_method) )
      {      
	XRAY__init_1(as_is(_nid), _method);
	XRAY__init_2(as_is(_nid), _method);
      } else {
	XRAY__init_1(as_is(_nid));
	XRAY__init_2(as_is(_nid));
      }
      
      return (1);
}
