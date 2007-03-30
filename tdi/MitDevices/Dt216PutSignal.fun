public fun Dt216PutSignal(in _nid, in _chan, in  _vins, in _data,  in _dim)
{
  _str=
    '_v1 = '//getnci(_vins, "FULLPATH")//'[(`2*_chan)], _v2 = '//getnci(_vins, "FULLPATH")//'[(`2*_chan+1)], '//
   'make_signal(build_with_units(_V1+($VALUE+32768)*(_v2-_v1)/65535,"volts"),'//
                 'build_with_units((`_data ),"counts"),(`_dim))';
  _signal=compile(_str);
  return(TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0)));
}
