public fun paragon_ftp_delete(in _url, in _delete)
{
  if (_delete)
    spawn("cd /tmp; ncftpget -DD '"//_url//"'");
  _ans="";
  for(_idx=0,_part=trim(element(_idx,"\\",_url));
      len(_part) > 0; _ans=_part,write(*,"_ans=",_ans),_idx++,_part=trim(element(_idx,"\\",_url)));
  _ans="/tmp/"//_ans//"\0";
  spawn("rm "//_ans);
  return(_ans);
}
