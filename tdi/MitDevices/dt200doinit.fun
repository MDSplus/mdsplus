/*
   Dt200DOInit.fun

   Function to send the digital output waveform(s) to a DT200 board.


   Josh Stillerman  2/3/02
*/
public fun Dt200DOInit(IN _board, IN _trigSrc, IN _clockSource, IN _bits, IN _times)
{
    _num = size(_bits)
    Dt200WriteMaster(_board, "setClock _clockSource");
    Dt200WriteMaster(_board, "setPhase DO P1 0");
    Dt200WriteMaster(_board, "setPhase DO P2 "//_num);
    Dt200WriteMaster(_board, "setEvent DO E1 EV_TRIGGER_FALLING "//_trigSrc);
    Dt200WriteMaster(_board, "setEvent DO E2 EV_TRUE");
    Dt200WriteMaster(_board, "setEvent DO E3 EV_NONE");

    /* pack the bits */
    _buf = (_times << 8) & (_bits & 0xFF);

    /* send the bits and times out */
    _brd = char(_board+ichar('0'));
    _devname = "/dev/acq32/acq32."//_brd//".DOf"; 
    _lun = MitDevicesIO->FOPEN(_devicename, 'w');
    MitDevicesIO->FWRITE(_buf, val(4), size(_buf))), val(_lun));
    MitDevicesIO->FCLOSE(val(_lun));
    /* return success */
  return(1);
}
