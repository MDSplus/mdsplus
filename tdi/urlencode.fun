public fun urlencode(in _text)
{

/* urlencode converts a text string into a string suitable to use in a url specification */

  _len=len(_text);
  _textout="";
  for (_i=0;_i<_len;_i++)
  {
    _char=ichar(extract(_i,1,_text));
    if ( (_char >= ichar('a') && _char <= ichar('z')) ||
         (_char >= ichar('A') && _char <= ichar('Z')) ||
         (_char >= ichar('0') && _char <= ichar('9')))
    {
      _textout=_textout//char(_char);
    }
    else
    {
      _textout=_textout//"%"//extract(_char/16,1,"0123456789ABCDEF")//extract(_char mod 16,1,"0123456789ABCDEF");
    }
  }
  return(_textout);
}
