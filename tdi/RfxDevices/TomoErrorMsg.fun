public fun TomoErrorMsg(in _code)
{

    _ERR_RESET_REPLY =  3;
    _ERR_CHAN_INACTIVE = 4;
    _ERR_SETPARAM_REPLY = 5;
    _ERR_STATUS_REPLY = 6;
    _ERR_GETPARAM_REPLY = 7;
    _ERR_NOT_SXRCHAN  = 8 ; 
    _ERR_ZERO_REPLY = 9 ;
    _ERR_FLAGS_REPLY = 10 ;  
    _ERR_ZERO_TIMEOUT = 11 ;
    _ERR_ZERO_NOSUCCESS = 12 ;   

	_ERR_TAXI_PROTOCOL	= 50;

    switch( _code )
	{
               case(_ERR_RESET_REPLY) return ("ERROR REPLY");
               case(_ERR_CHAN_INACTIVE) return ("Channel Inactive");
               case(_ERR_SETPARAM_REPLY) return ("Set parameter reply error");
               case(_ERR_STATUS_REPLY) return ("Status reply error");
               case(_ERR_GETPARAM_REPLY) return ("Get Parameter reply error");
               case(_ERR_NOT_SXRCHAN) return ("Not sxr channel");
               case(_ERR_ZERO_REPLY) return ("Zero reply error");
               case(_ERR_FLAGS_REPLY) return ("flags reply error");
               case(_ERR_ZERO_TIMEOUT) return ("Zero timeout error");
               case(_ERR_ZERO_NOSUCCESS) return ("Zero no success");
               case(_ERR_TAXI_PROTOCOL) return ("Taxi protocol error");
 	}	


   return ("Unknown error code : "//_code );

}
