public fun paragon_ftp_copy(in _url)
{
  spawn("cd /tmp; ncftpget -a '"//_url//"'");
  _ans="";
  for(_idx=0,_part=trim(element(_idx,"\\",_url));
      len(_part) > 0; _ans=_part,_idx++,_part=trim(element(_idx,"\\",_url)));
  return("/tmp/"//_ans//"\0");
}
