public fun Dt196AOSendFiles(in _board)
{
    _board_ip = getenv('BOARD'//trim(adjustl(_board)));
    write(*, 'tar -cvzf /tmp/waveForms.tgz /tmp/f.[0-1]0-9]');
    spawn('tar -cvzf /tmp/waveForms.tgz /tmp/f.[0-1][0-9]');
    write(*,"curl -s -T /tmp/waveForms.tgz -u ao: ftp://"//_board_ip//"/");
    spawn("curl -s -T /tmp/waveForms.tgz -u ao: ftp://"//_board_ip//"/");
    _cmd = "/ffs/untarWaves.sh";
    MdsValue('Dt200WriteMaster($, $, 1)', _board, _cmd);
    write(*,'rm -f /tmp/f.[0-1][0-9]');  
    spawn('rm -f /tmp/f.[0-1][0-9]'); 
    write(*,'rm -f /tmp/waveForms.tgz');  
    spawn('rm -f /tmp/waveForms.tgz'); 
 
}
