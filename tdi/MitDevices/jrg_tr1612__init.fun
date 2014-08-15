public fun JRG_TR1612__INIT(as_is _nid, optional _method) {
/*
 * $Id$
 *
 * TDI function to initialize the Joerger TR digitizers
 * (here named JRG_TR1612 - 16 channels, 12 bits)
 *
 * BAN/MBF 25-SEP-2003
 * BAN 20-JUL-2004: Put CAMAC functions in same order 
 *			as per Joerger manual, p. 35
 *			except with LAM support.
 *		Removed F16A4 function; not necessary
 *
 * Copyright (c) 2003
 * University of Washington
 * All rights reserved
 */

_name = DevNodeRef(_nid,1);  /* get name */

DevCamChk(_name,CamPiow(_name,0wu,9wu,_d=0wu,24wu),1,1);  /* master reset */
DevCamChk(_name,CamPiow(_name,0wu,10wu,_d=0wu,24wu),1,1);   /* clear LAM */

/* default values for control register */
_software_trigger = 1wu;
_front_pannel_trigger = 2wu;
_disarm_at_end_of_cycle = 4wu;
_wrap_mode = 8wu;
_pre_post_trigger = 64wu;
_24bit_mode = 8192wu;

_samples = DevNodeRef(_nid,7);  /* get samples */
_pre_trig_samp = DevNodeRef(_nid,8);  /* get pretrigger samples */

/* find post trigger samples (_post_trig_samp) */
_post_trig_samp = _samples - _pre_trig_samp;

/* set post trigger samples */
DevCamChk(_name,CamPiow(_name,3wu,16wu,_post_trig_samp,24wu),1,1);

/* set up the sampling clock */
_ext_clock = DevNodeRef(_nid,5);  /* get external clock if it is enabled*/
_is_ext_clock_enabled = getnci(_ext_clock,"LENGTH") > 0;
if (_is_ext_clock_enabled) _samp_interval_code = 8;
else {
  _samp_interval = DevNodeRef(_nid,4); /* read value of sampling interval set by user */
  _samp_interval_value = [2.5E-8,5.0E-8,1.0E-7,2.0E-7,4.0E-7,1.0E-6,1.0E-5,1.0E-4,0.0];
  for (_index=0; _index <= 8; _index++) {
    if (_samp_interval == _samp_interval_value[_index]) _samp_interval_code = _index; /* test for match */
    };
  };

/* check for bad clock */
_bad_clock = 0;
if_error( _samp_interval_code, _bad_clock = 1);
if( _bad_clock ) {
  write(*,"Bad clock value! "//_nid);
  return(0);
  };
  
/* set sampling clock interval */
DevCamChk(_name,CamPiow(_name,2wu,16wu,_samp_interval_code,24wu),1,1);

/* set control register code */
_control_register_code = _software_trigger + _front_pannel_trigger + 
      _disarm_at_end_of_cycle + _wrap_mode + _pre_post_trigger + _24bit_mode;
DevCamChk(_name,CamPiow(_name,1wu,16wu,_control_register_code,24wu),1,1);

/* reset Memory Location Counter to zero */
DevCamChk(_name,CamPiow(_name,2wu,9wu,_d=0wu,24wu),1,1);

/* start recording with Q confirmation (sometimes it doesn't stay init'd) */
_test_q = 0;
while(!_test_q) {
  wait(0.1);
  CamPiow(_name,0wu,25wu,_d=0wu,24wu);
  _test_q = CamQ();
  };

/* enable LAM */
DevCamChk(_name,CamPiow(_name,0wu,26wu,_d=0wu,24wu),1,1);

return(1);  /* return nicely*/

}
