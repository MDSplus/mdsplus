public fun chs_a14__wait(as_is _nid, optional _method)
{
  return(MdsVme->VmeWaitForInterrupt("/dev/dmaex0",val(3),val(0xce)));
}
