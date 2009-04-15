public fun Dt196AOSendFiles(in _board)
{
    _expr = "/sbin/ifconfig eth0 | grep 'inet addr' | awk -F: '{print $2}' | awk '{print $1}'";
    _board_ip = MdsValue("Dt200WriteMaster($ , $, 1)", _board, _expr);
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
