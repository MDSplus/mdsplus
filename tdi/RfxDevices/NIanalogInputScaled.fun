FUN PUBLIC NIanalogInputScaled(IN _sig, IN _coeff) 
{
    _data = data(_sig);
    _numSmp = size( _data );
    _data_scaled = zero( _numSmp , float(0) );
    _coeff_values = data( _coeff );

    _data_scaled = _coeff_values[0] + _coeff_values[1] * _data + _coeff_values[2] *  power( _data, 2 )  +  _coeff_values[3] * power( _data, 3 ) ;   	

/*    
    libNiInterface->xseries_AI_scale( _data,  ref(_data_scaled), val(_numSmp), _coeff_values);
*/
    return ( make_signal(_data_scaled,,dim_of(_sig)) );
}

