public fun ST133ReadData( in _shot )
{
    _file = "L:\\Taliercio\\devices\\ValisaCamera\\ccd_shot\\"//trim(adjustl(_shot))//".SPE";
	return ( ST133->ST133ReadData:dsc( _file ) );
}


