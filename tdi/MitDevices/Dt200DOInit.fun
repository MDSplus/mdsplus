/*
   Dt200DOInit.fun

   Function to send the digital output waveform(s) to a DT200 board.


   Josh Stillerman  2/3/02
*/
public fun Dt200DOInit(IN _board, IN _trigSrc, IN _clockSource, IN _bits, IN _times)
{
    _num = size(_bits);
    write (*, "starting DOinit  - num is "//_num);
    if (_clockSource == "INT") {
      _clockSource="INTERNAL_CLOCK";
    }
    Dt200WriteMaster(_board, "setClock DO "//_clockSource);
    Dt200WriteMaster(_board, "setPhase DO P1 0");
    Dt200WriteMaster(_board, "setPhase DO P2 "//_num);
    Dt200WriteMaster(_board, "setEvent DO E1 EV_TRUE");
    Dt200WriteMaster(_board, "setEvent DO E2 EV_TRIGGER_FALLING "//_trigSrc);
    Dt200WriteMaster(_board, "setEvent DO E3 EV_TRUE");

    /* pack the bits */
    _buf = (_times << 8) | (_bits & 0xFF);

     write (*, "Now lets send them ");
    /* send the bits and times out */
    _brd = char(_board+ichar('0'));
    _devname = "/dev/acq32/acq32."//_brd//".DOf"; 
    write(*, "open a lun to "//_devname);
    _lun = io->FOPEN(_devname, 'w');
    write(*, "It is open - now write");
    write(*, _buf);
    write(*, "kind is "//kind(_buf));
    io->FWRITE(_buf, val(4), val(size(_buf)), val(_lun));
    write(*, "it is written now close");
    io->FCLOSE(val(_lun));
    /* return success */
    write(*, "all done -- devname was "//_devname);
  return(1);
}
