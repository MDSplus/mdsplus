public fun CamMulti(in _routine, in _name, in _a, in _f, in _count, optional inout _data, in _mem, optional inout _iosb)
{
  _iosb = zero(4,0wu);
  if (_f < 8)
    _data = zero(_count,_mem < 24 ? 0wu : 0);
  return(build_call(8,'RemCamShr',_routine,_name,long(_a),long(_f),long(_count),ref(_data),long(_mem),ref(_iosb)));
}
