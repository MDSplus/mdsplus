public fun TomoGPIBChan(in _id)
{
   return( LONG( ((_id >> 33) & 0x3QU ) + 1 ) );
}
