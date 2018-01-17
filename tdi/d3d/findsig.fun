
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

   20100828 SMF - Remove FINDCER.fun call.  Use the tag names instead of
                  the old translation routine as cerprof.fun is retired.
	
   20140708 SMF - Added auditing to /var/log/mdsplus/findsig.log.  

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
	  if (EQ(machine(),"D3D")) {
	    _cmd = "echo '"//date_time()//" "//whoami()//" "//_tag//" "//_tree//"' >> /var/log/mdsplus/findsig.log &"; 
	    spawn(_cmd); 
          }
	  return (_node);
	}

        /* check transport */
        _stat=TreeShr->TreeOpen(ref("TRANSPORT\0"),val(_shotcheck));
        _close="TRANSPORT\0";

        /* search open tree(s) for _tag  */
        if (_stat) {
           _tree='';
           _stat=FINDSIGTAG(_tag, _tree, _node, _revert);
        }
        if (_closetree) {
           _dummy = TreeShr->TreeClose(ref(_close),val(_shotcheck));   /* DO NOT CLOSE TREE - performace hit!!! */
        }
	if (_stat) {
          if (EQ(machine(),"D3D")) {
	    _cmd = "echo '"//date_time()//" "//whoami()//" "//_tag//" "//_tree//"' >> /var/log/mdsplus/findsig.log &";
            spawn(_cmd);
          }
	  return (_node);
  	}

	/* Open D3D tree for next check */
	_stat=TreeShr->TreeOpen(ref("D3D\0"),val(_shotcheck));
	_close="D3D\0";

	/* search open tree(s) for _tag  */
	if (_stat) {
           _tree='';
	   _stat=FINDSIGTAG(_tag, _tree, _node, _revert);
	}

        if (_closetree) {
           _dummy = TreeShr->TreeClose(ref(_close),val(_shotcheck));   /* DO NOT CLOSE TREE - performace hit!!! */ 
        }

	/* if the signal was not found in any of the D3D subtrees, */
	/*    and it is not a special EFIT signal, check EFIT01    */

	if ( not(_stat) ) {

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
            if (EQ(machine(),"D3D")) {
	      _cmd = "echo '"//date_time()//" "//whoami()//" "//_tag//" "//_tree//"' >> /var/log/mdsplus/findsig.log &";
              spawn(_cmd);
            }
	    RETURN(_node);
	  }

	} ELSE {
	
	  ABORT();
	}

}
