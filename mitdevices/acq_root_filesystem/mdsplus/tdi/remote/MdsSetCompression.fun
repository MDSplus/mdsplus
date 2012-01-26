Public fun MdsSetCompression(in _level)
{
/* MdsSetCompression(in _level)

THIS ROUTINE IS USED INTERNALLY AND SHOULD NOT BE INVOKED DIRECTLY!!!

        TWFredian: [MIT, USA] October 2000
*/
  return(MdsIpShr->SetCompressionLevel(val(_level)));
}
