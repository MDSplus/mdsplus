public fun TR10Close(in _handle)
{
  TR10Error(TR10->TR10_Close(val(_handle)));
  _handle = 0L;
}
