public fun TR10GetState(in _handle)
{
  _state = 0b;
  TR10Error(TR10->TR10_GetState(val(_handle), ref(_state)));
  return(_state);
}
