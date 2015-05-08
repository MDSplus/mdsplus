public fun  PMAT_num_unit()
{
   _units = \RFX::PM_SETUP:UNITS;
    _num_units = 0;

   if( LEN(TRIM(_units)) == 0)
   {
      _num_units = 0;
   }
   else
   {
       _lidx = scan(_units, 'A', 1);
      _num_units =   _lidx / 3 + 1;
   }
   
   return ( _num_units );
   
}
