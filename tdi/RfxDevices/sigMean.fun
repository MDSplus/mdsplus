public fun sigMean(IN _sig, IN _npoint)
{
   _y = data( _sig );
   _x = dim_of( _sig );


   _size = size(_x);

   _nelem = _size / _npoint;

   _y = set_range( _npoint, _nelem, _y );

   _xo = [];
   _yo = [];

   for( _i = 0; _i < _nelem; _i++ )
   {
      _yo = [ _yo, mean( _y[*, _i] ) ];
      _xo = [ _xo, _x[ _i * _npoint + _npoint/2 ]  ];
   }

   return ( make_signal( _yo, , _xo) );

}
