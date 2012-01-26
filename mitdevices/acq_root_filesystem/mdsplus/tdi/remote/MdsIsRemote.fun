fun public MdsIsRemote()
/* mdsisremote()

	Return true if connected to remote node

call:
	_connected = mdsisremote();

	TWFredian: [MIT, USA] August 2000
*/
{
  _c = mdscurrent();
  return((len(_c) > 0) && (_c != "local"));
}
