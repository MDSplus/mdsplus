public fun TR10GetClockMode(in _handle, out _clockSource, out _divMode, out _divFactor)
{
  _clockSource = 0b;
  _PXIOut = 0b;
  _divMode = 0b;
  _Edge = 0b;
  _Termination = 0b;
  _divFactor = 0l;

  TR10Error(TR10->TR10_Clk_GetClockMode(val(_handle), _clockSource, _PXIOut, _divMode, _Edge, _Termination, _divFactor));
}
