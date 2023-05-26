public fun getSpiderGasType(optional in _shot)
{
   if(present(_shot))
       treeopen("spider", _shot);

   _tank3000 = data(build_path("\\GVS_TSHOT::NB_SIGV_GISA_TA00_STY:VAL"))[0];
   _tank3090 = data(build_path("\\GVS_TSHOT::NB_SIGV_GISA_TA90_STY:VAL"))[0];

   if(present(_shot))
       treeclose();

   if( _tank3000 != 0 && _tank3090 != 0 )
       return ( "unknown" );

   if ( _tank3000 == 1 ) return ("H2");
   if ( _tank3000 == 2 ) return ("D2");
   if ( _tank3090 == 1 ) return ("H2");
   if ( _tank3090 == 2 ) return ("D2");

   return("NoGas");

}

