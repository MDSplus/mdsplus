public fun PCAT_label()
{
   _config = data(build_path("\\RFX::POLOIDAL:PC_CONFIG"));

   _control = data(build_path("\\RFX::POLOIDAL:PC_CONTROL"));

    return( "[ "//_config//" "//_control//" ]" );

}



