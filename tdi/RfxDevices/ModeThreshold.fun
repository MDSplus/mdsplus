public fun ModeThreshold(in _amplitude)
{
	_status = MdsValue('variables->setFloatVariable("feedbackModeControlSwitchLevel", $1)', float(_amplitude));
	return (-100.);

}