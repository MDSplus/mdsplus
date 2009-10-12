public fun DevPutSignal1(in _nid, in _offset, in _coeff, in _data, in _lbound, in _ubound, in _dim)
{
  _data = set_range(_lbound : _ubound, _data);
  _signal_data = compile('build_with_units((`_coeff) * ((`_offset) + $VALUE),"volts"),');
  _signal_raw = compile('build_with_units((`_data ),"counts"),');
  _signal = make_signal(_signal_data, _signal_raw, _dim);   
  return(TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0)));
}
