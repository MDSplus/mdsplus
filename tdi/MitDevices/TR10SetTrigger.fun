public fun TR10SetTrigger(in _handle, in _trig_src, in _trig_chan, in _trig_output, in _trig_sense, in _trig_term, in _trig_sync, in _trig_level)
{
  switch(_trig_src) {
  case('ext') _TrigSrc =  _TR10_TRG_SOURCE_EXTERNAL; break;
  case('int') _TrigSrc = _TR10_TRG_SOURCE_INTERNAL; break;
  case('pxi') _TrigSrc =  _TR10_TRG_SOURCE_PXI; break;
  case('star') _TrigSrc =  _TR10_TRG_SOURCE_PXI_STAR; break;
  case DEFAULT Write(*, "TR10 : invalid Trigger source "//_clk_src); abort();
  };

  _TrigSense = (_trig_sense == '+') ? _TR10_TRG_RISING_EDGE : _TR10_TRG_FALLING_EDGE;
  _TrigTerm = (_trig_term) ? _TR10_TIG_TERMINATION_ON : _TR10_TRG_TERMINATION_OFF;
  _TrigSync = (_trig_sync) ? _TR10_TRG_SYNCHRONOUS : _TR10_TRG_ASYNCHRONOUS;
  /* what about level ??? */

  TR10Error(TR10->TR10_Trg_SetTrigger(val(_handle), val(_TrigSrc), val(_trig_chan), val(_trig_output), val(_TR10_TRG_EXT_OUT_ON),
                                      val(_TrigSense), val(_TrigTerm), val(_TrigSync)));
}
