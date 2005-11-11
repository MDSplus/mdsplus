public fun ST133ReadData( in _shot )
{
    _file = "C:\\Documents and
    settings\\spectroscopy\\CCD_experiment\\shotfiles\\CCD"//trim(adjustl(_shot))//".SPE";
	return ( ST133->ST133ReadData:dsc( _file ) );
}


