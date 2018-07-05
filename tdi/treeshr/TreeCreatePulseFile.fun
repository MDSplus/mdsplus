public fun TreeCreatePulseFile(in _shot, optional in _num, optional in _nids)
{
  if (! present(_num)) {
    _num=0;
    _nids=[0];
  };
  return(TreeShr->TreeCreatePulseFile(val(_shot),val(_num),ref(_nids)));
}
