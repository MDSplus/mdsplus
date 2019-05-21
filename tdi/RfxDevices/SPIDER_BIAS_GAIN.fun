public fun SPIDER_BIAS_GAIN(in _ps, in _control, in _enabled)
{

       if ( _enabled != "ENABLED")
             return( 0. );

       if ( _ps == "BI" )
       {
              if( _control == "CURRENT") {
                    return (5./600.);
              } if( _control == "VOLTAGE") {
                    return (5./30.);
              }
       } 

      if ( _ps == "BP" )
       {
              if( _control == "CURRENT") {
                    return (5./150.);
              } if( _control == "VOLTAGE") {
                    return (5./30.);
              }
       } 

       return ( 0. );

}
