public fun ST133ReadHeader( in _shot )
{
    _file = "C:\\Documents and
    settings\\spectroscopy\\CCD_experiment\\shotfiles\\CCD"//trim(adjustl(_shot))//".SPE";
write(*, _file);
	_out = ST133->ST133ReadHeader:dsc(_file);

	 if( size(_out) == 0 )
	   return ( 0 );
	 else
	   return ( _out );

}
