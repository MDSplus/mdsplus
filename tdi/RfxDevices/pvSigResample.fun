public fun pvSigResample(in _sig, in _tmsig, in _dt, in _t0, optional in _mode)
{
     if( !present(_mode))
         _s = pvResample(  _sig  , _dt, *, * , _t0);
     else
         _s = pvResample(  _sig  , _dt, *, * , _t0, _mode);

     _stm = pvResample( _tmsig, _dt, *,  *, _t0);

     return( sig_resample( _s, _stm ) );

}
