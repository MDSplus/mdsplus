public fun DevPutSignal(in _nid, in _offset, in _coeff, in _data, in _lbound, in _ubound, in _dim)
{
  _data = set_range(_lbound : _ubound, _data);
  _signal = compile(
    'build_signal(build_with_units((`_coeff) * ((`_offset) + $VALUE),"volts"),'//
                 'build_with_units((`_data ),"counts"),(`_dim))');
  return(TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0)));
}
