public fun spiderBurstClock(as_is _clock, as_is _burstTrig )
{
   _clockPath = (getnci(_clock,'fullpath'));
   _burstTrigPath = (getnci(_burstTrig,'fullpath'));
/*
   write(*, _clockPath, _burstTrigPath);
   _ckOut = compile("begin_of($1) + data($2) : end_of($1) + data($2) : slope_of($1)", _clockPath ,  _burstTrigPath );
*/
   _ckOut = compile("begin_of("//_clockPath//") + data("//_burstTrigPath//") : end_of("//_clockPath//") + data("//_burstTrigPath//") : slope_of("//_clockPath//")");
   return ( _ckOut );

}
