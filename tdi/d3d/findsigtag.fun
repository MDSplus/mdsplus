PUBLIC FUN FINDSIGTAG (IN _tag, INOUT _tree, OUT _node, OUT _revert)
/*
   Created: 98.03.09 Jeff Schachter
   Changes:
      2002.04.17 jrb changed _ctx=0l; to _ctx=0q;
*/
{
	_node = "";
	_revert=0;

	_ctx=0q;
	_node=TreeFindTagWild(_tag,_nid,_ctx);

    
	if (_node eq "") {
	  _stat = 0;
	} else { 

	  _stat=GETNCI(_node,"USAGE_SIGNAL");  

	  if (_stat) {
	    _p=INDEX(_node,":");

	    if (_tree eq "") {
	      _tree=EXTRACT(1,_p-1,_node);
	    }
	    _test=_node//":PTDATA_SIG";
	    _nid=0l;
            _revert=TreeShr->TreeFindNode(ref(_test//"\0"),ref(_nid));
	    _revert=WORD((_revert & 1) eq 1);
	    if (NOT(_REVERT)) {
	      _revert=0;
	    }
          }

	} 


	_dummy = TreeShr->TreeFindTagEnd(ref(_ctx));

	return(_stat);

}

