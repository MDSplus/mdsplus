public fun TR10SetClockMode(in _handle, in _clk_src, in _clk_output, in _div_mode, in _clk_sense, in _clk_term, in _clk_divide)
{
  
  switch(_clk_src) {
  case('ext') _ClkSrc =  _TR10_CLK_SOURCE_EXTERNAL; break;
  case('int') _ClkSrc = _TR10_CLK_SOURCE_INTERNAL; break;
  case('pxi') _ClkSrc =  _TR10_TRIG_SOURCE_PXI; break;
  case DEFAULT Write(*, "TR10 : invalid clock source "//_clk_src); abort();
  };

  _ClkOutput = (_clk_output) ? _TR10_CLK_PXI_TRIG7 : _TR10_CLK_NO_EXT_CLOCK;
  _DivMode = (_div_mode == 'div') ? _TR10_CLK_DIVIDE : _TR10_CLK_SUB_SAMPLE;
  _ClkSense = (_clk_sense == '+') ? _TR10_CLK_RISING_EDGE : _TR10_CLK_FALLING_EDGE;
  _ClkTerm = (_clk_term) ? _TR10_CLK_TERMINATION_ON : _TR10_CLK_TERMINATION_OFF;

  TR10Error(TR10->TR10_Clk_SetClockMode(val(_handle), val(_ClkSrc), val(_ClkOutput), val(_DivMode), val(_ClkSense), val(_ClkTerm), val(_clk_divide)));
}
