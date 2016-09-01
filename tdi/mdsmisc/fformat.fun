public fun fformat(in _value, optional in _width, optional in _precision)
{
/*  convert value to text using the equivalent of printf("%*.*g",_width,_precision,_value) */
  if (!present(_width)) _width = 12;
  if (!present(_precision)) _precision = 2;
  _width = (_width > 100) ? 100 : _width;
  _ft_value=ft_float(_value);
  return(MdsMisc->fformat:dsc(_ft_value,long(_width),long(_precision)));
}
