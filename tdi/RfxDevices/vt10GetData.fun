public fun vt10GetData(in _module, in _channel, in _samples, in _reduction)
{
    _data = zero([_samples], 0W);

    write(*, "Mod ", _module, " Ch ", _channel, " Samp ", _samples, " Redu ", _reduction);

    Tomo->vt10GetData(val(_module), val(_channel), val(_samples), val(_reduction), ref(_data));


     return ( _data );

}