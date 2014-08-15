FUN PUBLIC JavaReplaceSignal(in _idx, in _y, optional _opt_x, optional _opt_row, optional _opt_col, optional _opt_color, optional _opt_label)
{
    if(present(_opt_x))
        _x = float(_opt_x);
    else
        _x = dim_of(_y);
    if(present(_opt_row))
    {
        _row = long(_opt_row);
        _col = long(_opt_col);
    }
    else
    {
        _row = 1;
        _col = 1;
    }
    if(present(_opt_color))
        _color = _opt_color;
    else
        _color = 'black';
    if(present(_opt_label))
        _label = _opt_label;
    else
        _label = '';
    _status = JavaMds->removeAllSignals(val(_idx), val(long(_row)), val(long(_col)));
    if(_status == 0)
    	_status = JavaMds->addSignal(val(_idx), float(_x), fs_float(_y), val(long(kind(_x))), val(SIZE(_x)), val(long(_row)), val(long(_col)), _color, _label);
    return(_status);
}
