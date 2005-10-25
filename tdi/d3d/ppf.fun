public fun ppf(in _name, optional _shot)
{
   if (!present(_shot)) _shot = $SHOT;
   mdsconnect('mdsplus.jet.efda.org');
   return(SerializeIn(mdsvalue('SerializeOut(`(ppf("'//_name//'",'//text(_shot)//')))')));
}
