public fun Dt196AOSendFiles(in _board)
{
    _board_ip = getenv('BOARD'//trim(adjustl(_board)));
    write(*, "curl -T /tmp/f.[01-16] -u ao: ftp://"//_board_ip//"/AO/");
    spawn("curl -s -T /tmp/f.[01-16] -u ao: ftp://"//_board_ip//"/AO/");
    spawn('rm -f /tmp/f.[0-1][0-9]'); 
}
