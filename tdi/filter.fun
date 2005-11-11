FUN PUBLIC FILTER(in _signal, in _cut_off, optional  _num_poles)
{
	if(present(_num_poles))
        	RETURN ( MdsMisc->MdsFilter:DSC(FLOAT(_signal), FLOAT(DIM_OF(_signal), kind(0.)), SIZE(_signal), FLOAT(_cut_off), LONG(_num_poles)) );
	else
        	RETURN ( MdsMisc->MdsFilter:DSC(FLOAT(_signal), FLOAT(DIM_OF(_signal), kind(0.)), SIZE(_signal), FLOAT(_cut_off), 10) );
	
}

