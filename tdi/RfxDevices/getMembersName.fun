public fun getMembersName(in _path)
{

	 _member_num = GetNci( build_path(_path), "NUMBER_OF_MEMBER" );

	  if ( _member_num > 0  )
         {
		_nids_list = GetNci( build_path(  _path ), "MEMBER_NIDS" );
	           return (  GetNci( _nids_list , "NODE_NAME") );
	 }

         return ([]);
}
