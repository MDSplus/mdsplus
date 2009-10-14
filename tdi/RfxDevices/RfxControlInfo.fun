public fun RfxControlInfo(in _mode)
{
	switch( _mode )
	{
		case("TITLE")

		   if( getnci(\RFX::T_START_PR, "STATE") == 0)
 		    {
				_idx = execute("\\MHD_AC::CONTROL:TRIG1_CONTR");
				_title = "MHD_AC ["//RfxControlNameToIdx( _idx );
		    }
			else
				_title = "MHD_AC [ OFF";

		   if( getnci(\RFX::T_START_PR, "STATE") == 0)
 		    {
				_idx = execute("\\MHD_BC::CONTROL:TRIG1_CONTR");
				_title = _title//"] MHD_BC ["//RfxControlNameToIdx( _idx );
			}
			else
				_title = _title//"] MHD_BC [ OFF ";

		   _idx = execute("\\EDA1::CONTROL:TRIG1_CONTR");
			_title = _title//"] EDA1 ["//RfxControlNameToIdx( _idx );

		   _idx = execute("\\EDA3::CONTROL:TRIG1_CONTR");
			_title = _title//"] EDA3 ["//RfxControlNameToIdx( _idx )//"]";

            return (  _title );
	
		break;
		case("MHD_AC")

		   if( getnci(\RFX::T_START_PR, "STATE") == 0)
		    {
		        _idx = execute("\\MHD_AC::CONTROL:TRIG1_CONTR");
				return (  RfxControlNameToIdx( _idx ) );
			}
			else
			    return ("OFF");
		break;
		case("MHD_BC")
		   if( getnci(\RFX::T_START_PR, "STATE") == 0)
 		    {
		         _idx = execute("\\MHD_BC::CONTROL:TRIG1_CONTR");
				return (  RfxControlNameToIdx( _idx ) );
			}
			else
			    return ("OFF");
			
		break;
		case("EDA1")
		   _idx = execute("\\EDA1::CONTROL:TRIG1_CONTR");
            return (  RfxControlNameToIdx( _idx ) );		
		break;
		case("EDA3")
		   _idx = execute("\\EDA3::CONTROL:TRIG1_CONTR");
            return (  RfxControlNameToIdx( _idx ) );		
		break;
	}

}