FUN PUBLIC FILTER(in _signal, in _cut_off, optional  _num_poles)
{
	if(present(_num_poles))
        	RETURN ( MdsMisc->MdsFilter:DSC(_signal, DIM_OF(_signal), SIZE(_signal), FLOAT(_cut_off), LONG(_num_poles)) );
	else
        	RETURN ( MdsMisc->MdsFilter:DSC(_signal, DIM_OF(_signal), SIZE(_signal), FLOAT(_cut_off), 10) );
	
}

