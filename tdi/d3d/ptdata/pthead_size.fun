FUN PUBLIC PTHEAD_SIZE(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{

	_error=0;
	_size=ARRAY(7,0);

	_stat=libMdsD3D->mdsptheadsize_(_shot,_pointname//"          ",REF(_error),REF(_size));
	if (_error == 0) 
	  {
		return(_size);

	  } 
	else 
	  {
	    	return(ZERO(7,0));
	  }

}
	
