public fun NIADCBurstClock(as_is _clock, as_is _burstTrig )
{
   _clockPath = (getnci(_clock,'fullpath'));
   _burstTrigPath = (getnci(_burstTrig,'fullpath'));
   _ckOut = compile("begin_of("//_clockPath//") + data("//_burstTrigPath//") : end_of("//_clockPath//") + data("//_burstTrigPath//") : slope_of("//_clockPath//")");
   return ( _ckOut );

}
