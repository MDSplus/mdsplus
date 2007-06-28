public fun MCAMERAErrorMessage(in _camId )
{
	_msg = repeat(' ', 1024);
	NI_PCI_14XX->cameraHWErrorMsg( val( _camId ), ref(_msg) );
	return ( trim( _msg ) );
}