public fun TR10Open(in _board)
{
  if (not Allocated('_TR10Initialized')) {
    write (*, "set up the initial constants");
    public _TR10Initialized = 0x1;
    public _TR10_IDLE = 0x1;
    public _TR10_PRE_TRIGGER = 0x2;
    public _TR10_POST_TRIGGER = 0x4;
    public _TR10_IO_ERROR_CLOCK = 0x1;
    public _TR10_IO_ERROR_TRIGGER = 0x2;
    public _TR10_CLK_SOURCE_INTERNAL = 0x0;
    public _TR10_CLK_SOURCE_PXI = 0x1;
    public _TR10_CLK_SOURCE_EXTERNAL = 0x2;
    public _TR10_CLK_NO_EXT_CLOCK = 0x0;
    public _TR10_CLK_PXI_TRIG7 = 0x1;
    public _TR10_CLK_DIVIDE = 0x0;
    public _TR10_CLK_SUB_SAMPLE = 0x1;
    public _TR10_CLK_RISING_EDGE = 0x0;
    public _TR10_CLK_FALLING_EDGE = 0x1;
    public _TR10_CLK_TERMINATION_OFF = 0x0;
    public _TR10_CLK_TERMINATION_ON = 0x1;
    public _TR10_TRG_SOURCE_INTERNAL = 0x0;
    public _TR10_TRG_SOURCE_PXI = 0x1;
    public _TR10_TRG_SOURCE_PXI_STAR = 0x2;
    public _TR10_TRG_SOURCE_EXTERNAL = 0x3;
    public _TR10_TRG_EXT_OUT_OFF = 0x0;
    public _TR10_TRG_EXT_OUT_ON = 0x1;
    public _TR10_TRG_FALLING_EDGE = 0x0;
    public _TR10_TRG_RISING_EDGE = 0x1;
    public _TR10_TRG_TERMINATION_OFF = 0x0;
    public _TR10_TRG_TERMINATION_ON = 0x1;
    public _TR10_TRG_SYNCHRONOUS = 0x0;
    public _TR10_TRG_ASYNCHRONOUS = 0x1;
    TR10->TR10_InitLibrary();
  }
  _handle = 0l;
  TR10Error(TR10->TR10_Open(val(_board), ref(_handle)));
  return(_handle);
}
