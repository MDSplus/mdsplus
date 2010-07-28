public fun bRadField2D( in _polArray )
{

	_signals = [ ];

		for( _k = 1; _k <= 48; _k++)
		{
	
			if( _k < 10)
			{
				_sigName = "\\mhd_br::VMBRX0"//trim(adjustl(_k))//trim(adjustl( _polArray ))//"_VI2VA";
			}
			else
			{
				_sigName = "\\mhd_br::VMBRX"//trim(adjustl(_k))//trim(adjustl( _polArray ))//"_VI2VA";
			}

			_signal = execute(_sigName);

			_signals = [_signals, data( _signal )];
			
		}
		
		
		_x = data(dim_of( _signal ));

	return ( make_signal( _signals ,, _x, [1..48] ) );

}