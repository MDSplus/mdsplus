public fun hdf5read(in _name)
{
  _ans = *;
  hdf5tdi->hdf5read(_name,xd(_ans));
  return(_ans);
}
