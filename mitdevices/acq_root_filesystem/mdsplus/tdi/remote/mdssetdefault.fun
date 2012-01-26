fun public mdssetdefault(in _path)
/*	mdssetdefault(in _path)

	Set default path in tree
	
call:   
	_path		= string, pathname

	TWFredian: [MIT, USA]	August 2000
*/
{
     return(mdsvalue("TreeSetDefault($)",_path));
}
