FUN PUBLIC NIpxi6259AnalogInputScaled(IN _sig, IN _coeff, IN _gain) 
{
    _data = data(_sig);
    _numSmp = size( _data );
    _data_scaled = zero( _numSmp , float(0) );
    _coeff_values = data( _coeff );
    _n_coeff = size( _coeff_values );


    _gain_divider = [1.0, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0];
/*
                       na  10V,  5V, 2V,  1V, 500mv, 200mv, 100mv
*/
    _data = data(_sig);
    _numSmp = size( _data );
    _data_scaled = zero( _numSmp , float(0) );
    _coeff_values = data( _coeff );
    _n_coeff = size( _coeff_values );

    _data_scaled = _coeff_values[ _n_coeff - 1 ];
    for( _c = _n_coeff - 2 ; _c >= 0 ; _c-- )
    {
	_data_scaled *= _data;
	_data_scaled += _coeff_values[ _c ];
    }

    _data_scaled = _data_scaled /  _gain_divider [ _gain ];


/*
    libNiInterface->pxi6259_ai_polynomial_scaler( _data, ref(_data_scaled), val(_numSmp), _coeff_values, val(_n_coeff-1), val(_gain));
*/
    return ( make_signal(_data_scaled,,dim_of(_sig)) );


}

