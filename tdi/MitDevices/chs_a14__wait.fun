public fun chs_a14__wait(as_is _nid, optional _method)
{
  return(MdsVme->WaitForInterrupt("/dev/vmp0",val(3),val(0xce),"",val(0),val(0)));
}
