public fun TomoGPIBSlot(in _id)
{
   return( LONG( (_id >> 28) & 0x1FQU ) );
}
