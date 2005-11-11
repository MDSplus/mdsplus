public fun mdssig(in _signal, optional _shot)
{
   if (!present(_shot)) _shot = $SHOT;
   _path = findsig(_signal);
   _stat=TreeShr->TreeOpen(ref("D3D\0"),val(_shot));
   _x = data(build_path(_path));
   return(_x);
}
