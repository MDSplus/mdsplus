public fun TR10GetClockDiv(in _handle)
{
  _clockSource = 0b;
  _PXIOut = 0b;
  _divMode = 0b;
  _Edge = 0b;
  _Termination = 0b;
  _divFactor = 0l;

  TR10Error(TR10->TR10_Clk_GetClockMode(val(_handle), ref(_clockSource), ref(_PXIOut), ref(_divMode), ref(_Edge), ref(_Termination), ref(_divFactor)));
  return(_divFactor);
}
