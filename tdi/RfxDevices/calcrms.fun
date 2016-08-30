public fun calcrms(in _sig )
{
    _rms = maxval( sqrt( integrate ( make_signal( data( _sig ) * data( _sig ), *, dim_of( _sig ) ) )   /  maxval( dim_of( _sig ) ) ) ) ;

    return ( _rms );
}
