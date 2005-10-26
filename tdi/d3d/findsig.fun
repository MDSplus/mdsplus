
PUBLIC FUN FINDSIG (IN _tag, OPTIONAL OUT _tree, OPTIONAL OUT _revert,
		    OPTIONAL IN _no_ptdata, OPTIONAL IN _closetree)

/* This function finds the subtree containing _tag.  It assumes:
	- Every tag name is unique in the MDSplus tree
	- With the exception of the PTDATA signals, there is no
	  tag name that is the same as a PTDATA pointname or pseudopointname
	  (always check POINTNAME.DAT and GETALLDAT.F before adding new tags)

	- if _no_ptdata is true, if the tree name is PTDATA the signal
	  will not be recognized

   Side effects:  Opens and closes the D3D model, or either the EFIT01 or
		  EFIT02 models.

   Created: 98.02.03  Jeff Schachter
*/

{

	IF (NOT PRESENT(_no_ptdata)) _no_ptdata=0;

	_revert=0l;
	_tree='';
	_node='';
	_shotcheck=-1;
        if (not present(_closetree)) _closetree=0l;


	/* check to see if tag is a special tag */

	_tag=UPCASE(TRIM(_tag));

	  /* check TS first, if it is a TS signal, RETURN NOW!!! */

	_stat = FINDTS(_tag, _tree, _node, _revert);

	if (_stat) {
	  return (_node);
	}

	_stat = FINDCER(_tag, _tree, _node, _revert);
        if (_stat) {
          return (_node);
        }

	/* Open D3D tree for next check */
	_stat=TreeShr->TreeOpen(ref("D3D\0"),val(_shotcheck));
	_close="D3D\0";

	/* search open tree(s) for _tag  */

	if (_stat) {
	   _stat=FINDSIGTAG(_tag, _tree, _node, _revert);
	}

        if (_closetree) {
           _dummy = TreeShr->TreeClose(ref(_close),val(_shotcheck));   /* DO NOT CLOSE TREE - performace hit!!! */ 
        }

	/* if the signal was not found in any of the D3D subtrees, */
	/*    and it is not a special EFIT signal, check EFIT01    */


	if (not(_stat)) {

	  /* Now check EFIT */
	  _stat = FINDEFIT(_tag, _tree);
	  if (_stat) {
	    /* signal is a special EFIT signal, so open EFIT tree */
	    _stat=TreeShr->TreeOpen(ref(_tree//"\0"),val(_shotcheck));
	    _close=_tree;
            if (_stat) {
	      _stat=FINDSIGTAG(_tag, _tree, _node, _revert);
            }
	  } 
        } 

	/* if the signal was not found in any of the D3D subtrees, */
	/*    and it is not a special EFIT signal, check EFIT01    */


	if (not(_stat) && (_tree eq "") ) {
	  _stat = TreeShr->TreeOpen(ref("EFIT01\0"),val(_shotcheck));
	  if (_stat) {
	    _stat=FINDSIGTAG(_tag, _tree, _node, _revert);
          }
          if (_closetree == 1) {
	    _dummy = TreeShr->TreeClose(ref("EFIT01\0"),val(_shotcheck)); 
          }
	}

	IF (_stat) { 
	  if ((_tree eq "PTDATA") && (_no_ptdata)) {
	    ABORT(); 
	  } else {
	    RETURN(_node);
	  }

	} ELSE {
	
	  ABORT();
	}

}
