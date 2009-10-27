public fun RfxUnitNumber(in _units, in _unitType)
{
   _lidx = scan(_units, _unitType, 1);

write(*, _units, _unitType, _lidx);

if(_lidx < 0)
   {
      _num_units = 0;
   }
   else
   {
      _num_units =   _lidx / 3 + 1;
   }

   return ( _num_units );
}
