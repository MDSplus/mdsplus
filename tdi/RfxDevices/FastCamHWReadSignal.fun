public fun FastCamHWreadSignal(in _comp_mode, in _num_samples, in _lens_type, in _aperture, in _f_distance, in _filter)
{

	write(*, "Lens Type ", _lens_type);
	write(*, "Aperture ", _aperture);
	write(*, "F Distance ", _f_distance);
	write(*, "Filter ", _filter);

	_buf = array(_num_samples, 0e0);
	_nSts = FastCam->FastCamReadSignal( val( _comp_mode ), val( _num_samples ), ref( _lens_type ), val( _aperture ), val( _f_distance ), ref( _filter ), ref ( _buf ) );
	if( _nSts < 0 )
		return ( _nSts );

	return ( _buf );
}
