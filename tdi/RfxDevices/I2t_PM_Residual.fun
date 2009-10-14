public fun I2t_PM_Residual(in _shots, in _temp)
{
	_I2t_MAX = 23e9;
	
	_i2t = _I2t_MAX - ( ( _temp - 21 ) * 385 * 8960 * (0.042 * 0.05) * (0.042 * 0.05) ) / 2e-8;

	_ns = size( _shots );
	
	for( _i = 0; _i < _ns; _i++ )
	{
		_status = tcl('set tree rfx/shot='//_shots[ _i ]);
		write(*, "Shot = ", _shots[ _i ] , _i, _status);
		_val = if_error( 0.25*(integrate(resample(build_path("\\edam::pbmc01_im01va"),,,0.01)^2)+integrate(resample(build_path("\\edam::pbmc02_im01va"),,,0.01)^2)+integrate(resample(build_path("\\edam::pbmc03_im01va"),,,0.01)^2)+integrate(resample(build_path("\\edam::pbmc04_im01va"),,,0.01)^2)), [0]);
		_i2t = _i2t - _val[ size(_val) - 1 ];
	}
	Write("Residual I2t Magnetizing : ", _i2t);
	return ( _i2t ) ;
}