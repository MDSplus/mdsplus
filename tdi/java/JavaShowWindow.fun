FUN PUBLIC JavaShowWindow(in _idx, in _x, in _y, in _width, in _height)
{
	RETURN(JavaMds->showWindow(val(long(_idx)), val(long(_x)), val(long(_y)), val(long(_width)), val(long(_height))));
}