public fun pulseCheck_PMAT()
{
   _units = \RFX::PM_SETUP:UNITS;
   
   _lidx = scan(_units, 'A', $TRUE);
   if(_lidx < 0)
   {
      _num_units = 0;
   }
   else
   {
      _num_units =   _lidx / 3 + 1;
   }
   
   write(*, "Numero di unita' ", _units, _num_units);
   
   _max_curr = \RFX::PM_SETUP:MAX_I_MAG;

   _max_val = maxval(\RFX::PM_SETUP:WAVE);
   
   if(_max_val * _num_units > _max_curr)
   {
 	   return (1);
   }

   return(0);

}



