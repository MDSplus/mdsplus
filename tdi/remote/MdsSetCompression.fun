Public fun MdsSetCompression(in _level)
{
  return(MdsIpShr->SetCompressionLevel(val(_level)));
}
