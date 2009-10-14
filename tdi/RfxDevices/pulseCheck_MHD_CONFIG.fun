public fun pulseCheck_MHD_CONFIG()
{
	for( _i = 0; _i < 16; _i++)
	{
	
		_path = "\\RFX::PR_CONFIG:REC_"//trim(adjustl(_i * 12 + 1))//"_"//trim(adjustl(_i * 12 + 12));
		write(*, "PATH =", _path);
		
		_paramAll = data(build_path(_path));
		
		if(mod( _i, 4) == 0)
		{
			write(*, "\tReference PATH =", _path, _i);
			_paramRef = _paramAll[12 * (0..44) + 0];
		}

		for(_j = 0; _j < 12; _j++)
		{
			if( sum( _paramRef != _paramAll[12 * (0..44) + _j] ) != 0 )
			{
				write(*, "Config chopper "//trim(adjustl( _i * 12 + 1 + _j ))//" anomala ");
				_param = _paramAll[12 * (0..44) + _j];
				for( _k = 0; _k < 45 ; _k++)
				{
					if(_paramRef[_k] != _param[_k] )
					{
						write(*, "\tAnomalia nel parametro ", _k, _paramRef[_k], _param[_k]);
						
					}
				}
				
			}
		}
	}
}