public fun sigBit(in _sig, in _bit)
{

    _data = ( long(data(_sig)) & (1 << _bit) ) >> _bit;

    return ( make_signal(_data,,dim_of(_sig)) );

}
