fun Dt100WriteAO(in _board, in _buf)
{
  write(*, "starting Dt100WriteAO");
  _brd = char(_board+ichar('0'));
  _devname = "/dev/acq32/acq32."//_brd//".AOfrt";
  _lun = MitDevices->FOPEN(_devname, "w+");
  write(*, "The file is open");
  _len = Len(_buf);
  _sz = Size(_buf);
  MitDevices->FWRITE(_buf, VAL(_len), VAL(_sz), VAL(_lun));
  write (*, "the data is written");
  MitDevices->FCLOSE(val(_lun));
  write (*, "returning");
  return(1);
}
