public fun TomoChanIsActive(in _id)
{
   return( LONG ( (QUADWORD(_id) >> 41) & 0x1UQ ) );
}
