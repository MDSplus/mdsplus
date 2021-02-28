public fun SPIDER_BIAS_GAIN(in _ps, in _control, in _enabled)
{
/*
2019 10 30
ATTENZIONE - Modificato scaling della tesione da 5V -> 30V a 5V -> 50V
*/

       if ( _enabled != "ENABLED")
             return( 0. );

       if ( _ps == "BI" )
       {
              if( _control == "CURRENT") {
                    return (5./600.);
              } if( _control == "VOLTAGE") {
                    return (5./50.);
              }
       } 

      if ( _ps == "BP" )
       {
              if( _control == "CURRENT") {
                    return (5./150.);
              } if( _control == "VOLTAGE") {
                    return (5./50.);
              }
       } 

       return ( 0. );

}
