FUN PUBLIC JavaDim(in _x, in _xmin, in _xmax)
{
        _xsamples = SIZE(_x,);
        RETURN(JavaMds->JavaDim:DSC(_x, _xsamples, _xmin, _xmax));
}
