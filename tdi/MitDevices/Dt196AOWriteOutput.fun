public fun Dt196AOWriteOutput(_board, _chan, _wave)
{
  _filenam = 'f.'//trim(adjustl(_chan/10))//trim(adjustl(_chan mod 10));
  _lun = MitDevicesIO->FOPEN('/tmp/'//_filenam, 'w');
  MitDevicesIO->FWRITE(_wave, val(len(_wave)), val(size(_wave)), val(_lun));
  MitDevicesIO->FCLOSE(val(_lun));
  _board_ip = getenv('BOARD'//trim(adjustl(_board)));
  write(*, "curl -T "//_filenam//" -u ao: ftp://"//_board_ip//"/AO/"//_filenam);
  spawn("curl -T /tmp/"//_filenam//" -u ao: ftp://"//_board_ip//"/AO/"//_filenam);
/*  spawn('rm -f /tmp/'//_filenam);*/
}
