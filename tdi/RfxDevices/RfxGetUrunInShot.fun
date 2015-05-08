public fun RfxGetUrunInShot(in _mode)
{
   _uruns = if_error( data(build_path("\\VERSIONS::URUNS")),,[]);
   _nUruns = size(_uruns);

   if( _nUruns > 0 )
   {
	if( _mode == "PREVIOUS" )
	{
	   if( _nUruns - 1 > 0 )
	      return ( _uruns[ (_nUruns - 1) - 1 ] );
	   else
	      return ( 1 );
	}

	if( _mode == "LAST" )
	{
	   return ( _uruns[ (_nUruns - 1) ] );
	}	
   } 	
   return ( 1 );	
}
