fun public mdsopen(in _tree, in _shot)
/*	mdsopen(in _tree, in _shot)

	Open an mds tree
	
	Note: successful mdsconnect() call required

call:   
	_tree		= treename
	_shot		= shot 

	TWFredian: [MIT, USA]	August 2000
*/
{
     return(mdsvalue("TreeOpen($,$)",_tree,_shot));
}
