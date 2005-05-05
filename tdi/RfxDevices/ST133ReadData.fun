public fun ST133ReadData( in _shot )
{
    _file = "R:\\ricercatori\\FA\\CCD_SHOT\\ "//trim(adjustl(_shot))//".SPE";
	return ( ST133->ST133ReadData:dsc( _file ) );
}


