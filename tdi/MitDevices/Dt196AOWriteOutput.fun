public fun Dt196AOWriteOutput(_board, _chan, _wave)
{
  /* write(*, 'startiong WriteOutput '//_chan); */
  _filenam = 'f.'//trim(adjustl(_chan/10))//trim(adjustl(_chan mod 10));
  /* write(*, 'filename = '//_filenam); */
  _lun = MitDevicesIO->FOPEN('/tmp/'//_filenam, 'w');
  if (_lun != 0) {
    /* write(*, '_lun = '//_lun); */
    MitDevicesIO->FWRITE(_wave, val(len(_wave)), val(size(_wave)), val(_lun));
    /* write(*, "Write done"); */
    MitDevicesIO->FCLOSE(val(_lun));
    /* write(*, "Close done"); */
/*
    _board_ip = getenv('BOARD'//trim(adjustl(_board)));
    write(*, "curl -T /tmp/"//_filenam//" -u ao: ftp://"//_board_ip//"/AO/"//_filenam);
    spawn("curl -s -T /tmp/"//_filenam//" -u ao: ftp://"//_board_ip//"/AO/"//_filenam);
    spawn('rm -f /tmp/'//_filenam); 
*/ 
 } else {
    write(*, "Error opening "//_filenam//" for write");
  }
}
