/* 
   sig_resample.fun

   function to resample a signal to the timebase
   of another signal.

   usage:
       sig_resample( signal_to_resample, timebase_source_signal)

   where:
      signal_to_resample - signal which will be resampled to the timebase 
                           of the second argument.
      timebase_source_signal - signal whose timebase will be used to resample
                           the first signal

   example:
      \ip - sig_resample(\bt, \ip)
      subtracts \bt from \ip after first putting both signals onto the  timebase of \ip


   Josh Stillerman 4/6/01
*/	
FUN PUBLIC SIG_RESAMPLE(IN _src, IN _tmsrc)
{
  _x = DATA(DIM_OF(_tmsrc));
  return(make_signal(MdsMisc->Resample:DSC(XD(_src),XD(_x)), * , _x));
}
