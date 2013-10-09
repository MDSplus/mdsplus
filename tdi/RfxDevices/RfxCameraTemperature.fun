/*      

    RfxCameraTemperature.fun

	la funzione restituisce il segnale che ha per ascissa
	la posizione toroidale della sonda di temperatura e per 
	ordinata l'ultimo valore della temperatura nel dato run 
	per una data posizione poloidale.
	I parametri da passare sono :
	
	_sonda : tipo di sonda da utilizzare TA, TL, TE, TF, TS o TB
	_pol : posizione poloidale valore tra 1 e 8 
	       che individua le seguenti posizioni poloidali
			
		   [6.43,  22.5,  67.5,  112.5,  157.5,  173.57,  186.43,  202.5, 247.5, 292.5, 337.5, 353.57]

*/

public fun RfxCameraTemperature(in _sonda,  in _pol, in _mode)
{	
	_x = [];
	_y = [];

/*				0	1	2	   3		4	   5	        6             7         8         9         10       11 */		
	_POL_AR = [6.43,  22.5,  67.5,  112.5,  157.5,  173.57,  186.43,  202.5, 247.5, 292.5, 337.5, 353.57];
	_TA_POL = [0,       1,      2,         3,        4,          0,           0,           5,         6,       7,        8,          0];
	_TB_POL = [1,       0,      0,         0,        0,          4,           5,           0,         0,       0,        0,          0];
	_TE_POL = [1,       0,      0,         0,        0,          4,           5,           0,         0,       0,        0,          8];
	_TL_POL = [0,       1,      2,         3,        4,          0,           0,           5,         6,       7,        8,          0];
	

	_probe_arr = execute( compile ("\"_"//_sonda//"_POL\"") );

	if ( _pol < 0 || _pol > 11 ||  _probe_arr[ _pol ] == 0 )
		return ( * );

	if( _mode == "LABEL")
		return ( "Probe "//_sonda//" Pol. "//_POL_AR[ _pol ]);

	if( _mode != "SIGNAL")
		return ( * );


        for( _i = 1; _i <= 72; _i++)
	{
	    if(_i <= 9)
		   _tag = "\\"//_sonda//"0"//TEXT(_i, 1)//TEXT(  _probe_arr[ _pol ], 1);
		else
		   _tag = "\\"//_sonda//TEXT(_i, 2)//TEXT( _probe_arr[ _pol ] , 1);
		
		_error = 0;
		_data = if_error( data(build_path(_tag)), _error = 1);
		if( !_error)
		{

write(*, "Tags ", _tag);

		   _x = [ _x, ( _i - 1 ) * 5 ];
		   _y = [ _y,  _data[ size(_data) - 1 ]  ];	
		}
	}

	if( size (_y) == 1 )
	{
		_x = [ _x, _x[0] ];
		_y = [ _y, _y[0] ];
    	}

        return( make_signal(_y,,_x) );

}
