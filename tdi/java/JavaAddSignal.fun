FUN PUBLIC JavaAddSignal(in _idx, in _x, in _y, in _row, in _col, in _color, in _label)
{
	RETURN(JavaMds->addSignal(_idx, _x, _y, SIZE(_x), _row, _col, _color, _label));
}