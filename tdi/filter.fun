FUN PUBLIC FILTER(in _signal, in _cut_off)
{
        RETURN ( MdsMisc->MdsFilter:DSC(_signal, DIM_OF(_signal), SIZE(_signal), FLOAT(_cut_off)) );
}

