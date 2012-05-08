/*      

    RfxMaxTemperature.fun

	la funzione restituisce il segnale che ha per ascissa
	la posizione toroidale della sonda di temperatura e per 
	ordinata il valore massimo della temperatura nel dato run 
	per una data posizione poloidale.
	I parametri da passare sono :
	
	_sonda : tipo di sonda da utilizzare TA, TF, TS o TB
	_pol : posizione poloidale valore tra 1 e 8 
	       che individua le seguenti posizioni poloidali
			
		   [22.5, 67.5, 112.5, 157.5, 202.5, 247.5, 292.5, 337.5]

*/

public fun RfxMaxTemperature(in _sonda, in _pol,  optional in _last)
{	
	_x = [];
	_y = [];

    for( _i = 1; _i <= 72; _i++)
	{
	    if(_i <= 9)
		   _tag = "\\"//_sonda//"0"//TEXT(_i, 1)//TEXT(_pol, 1);
		else
		   _tag = "\\"//_sonda//TEXT(_i, 2)//TEXT(_pol, 1);
		
		_error = 0;
		_data = if_error( data(build_path(_tag)), _error = 1);
		if( !_error)
		{

/*
write(*, "Tags ", _tag);
*/
		   _x = [ _x, ( _i - 1 ) * 5 ];
		  if( present( _last ) )
		       _y = [ _y,  _data[ size(_data) - 1 ]  ];	
		  else
		       _y = [ _y,  maxval( _data ) ];
		}

	}

    return( make_signal(_y,,_x) );

}
