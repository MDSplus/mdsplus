public fun DevPutSignalNoBounds(in _nid, in _offset, in _coeff, in _data,  in _dim)
{
  _signal = compile(
    'build_signal(build_with_units((`_coeff) * ((`_offset) + $VALUE),"volts"),'//
                 'build_with_units((`_data ),"counts"),(`_dim))');
  return(TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0)));
}
