public fun TomoVMEChan(in _id)
{
   return( LONG( ((_id >> 5) & 0x3QU ) + 1 ) );
}
