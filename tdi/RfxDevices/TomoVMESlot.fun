public fun TomoVMESlot(in _id)
{
   return( LONG( (_id >> 1) & 0xFQU ) );
}
