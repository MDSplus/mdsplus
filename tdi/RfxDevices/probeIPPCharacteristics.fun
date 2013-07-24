public fun probeIPPCharacteristics( in _idx , in _v_offset, in _i_offset,  optional  _isLabel )
{
   _bTime = [1.75 + _idx * 0.5 :1.75 + ( _idx+1 ) * 0.5 : * ];

    _t_start =  1.75 + _idx * 0.5;
    _t_end = _t_start +  0.5;
	
    if( present( _isLabel ) )
        return ( "t start "//trim(adjustl(_t_start))//" t end "//trim(adjustl(_t_end))//" V offset "//trim(adjustl(_v_offset)) //" I offset "//trim(adjustl(_i_offset))   );

     _curr     = filter(\IPP_PROBES::CURRENT, 1000, 2)[_t_start  : _t_end : *] + filter((\IPP_PROBES::VOLTAGE + _v_offset), 1000, 2)[_t_start  : _t_end : *]/105000. - _i_offset;
     _voltage  = filter(-\IPP_PROBES::VOLTAGE, 1000, 2)[_t_start  : _t_end : *] + _v_offset;

    _sig = make_signal( _curr,, _voltage);

     return( _sig );

}
