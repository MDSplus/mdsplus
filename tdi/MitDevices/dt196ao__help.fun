public fun DT196AO__HELP(as_is _nid, optional in _method)
{
  _ans = 'D-Tacq Solutions Model 196 digitizer - Arbitrary Waveform Generator function\n'//
	'Help provided in liu of device setup  - 1/16/07\n'//
	' \n'//
	'Device fits user provided waveforms to up to 16K points sampled at a subsampled\n'//
	'copy of the data acquisition clock\n'//
        ' \n'//
        ' \n'//
        '  Main Controls : \n'//
        ' \n'//
	' DI0 - DI5  - expressions for the times of the signals on these internal wires.\n'//
	'               EG DI0 - clock expression, DI3 - trigger expression\n'//
	' AO_CLK - Internal wire to use for clock - default "DI0"\n'//
        ' AO_TRIG - Internal wire to use for the trigger - default "DI3"\n'//
	' FAWG_DIV - divisor for AO_CLK - defualt 20.\n'//
        ' CYCLE_TYPE - "ONCE" or "CYCLIC"\n'//
        ' TRIG_TYPE - "HARD_TRIG" or "?"\n'//
        ' MAX_SAMPLES - maximum number of samples to fit to userwaveforms < 16385\n'//
	' OUTPUT_01... - signal (-10 to 10 V) vs Time in seconds.\n'//
	'                The timebase must be resolvable\n'//
	' OUTPUT_01:FIT - "LINEAR" or "SPLINE"';

  write(*, _ans);
  return(1);
}
         
