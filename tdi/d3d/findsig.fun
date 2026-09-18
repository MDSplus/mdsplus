/* 

   FINDSIG()

   This function finds the subtree containing a specified MDSPLus tag (_tag). 

   Search Order:
      - Special case: TS pointnames via findts()
      - Special case: Confinement pointames via the TRANSPORT tree
      - Default: Search D3D model tree and all of its sub-trees
      - Special case: EFIT pseudo pointnames via findefit()
      - Special case: EFIT01 model tree 
      - Unfound: Exit; the client application can make any relevant ptdata calls itself

   Expectations:
      - Any mdsplus tag names, pseudo-tag names, or ptdata pointnames are unique and unambiguous

   Side Effects:
      - For performance, opened trees are not closed unless the _closetree argument is specified

   Logging Level:
      - 0: disabled
      - 1: all requests
      - 2: only discovered tags
 
*/

PUBLIC FUN FINDSIG (IN _tag, OPTIONAL OUT _tree, OPTIONAL OUT _revert,
		    OPTIONAL IN _no_ptdata, OPTIONAL IN _closetree)
{


        PRIVATE FUN FS_LOGGING(IN _level, IN _tag, IN _tree) {
           /* Only log if the server is $MACHINE=D3D */
           IF (EQ(machine(),"D3D")) {
              _logfile='/var/log/mdsplus/findsig.log';
              _cmd = "echo "//_level//" '"//date_time()//" "//whoami()//" "//_tag//" "//_tree//"' >> "//_logfile//" &";
              spawn(_cmd);
           }
           return(1);
        }


        /* Default Settings */
	if (not present(_no_ptdata)) _no_ptdata=0;
        if (not present(_closetree)) _closetree=0l;
	_revert=0l;
	_tree='';
	_node='';
	_shotcheck=-1;
        _logging_level=2;
	_tag=UPCASE(TRIM(_tag));

	/* Special Case - TS */
	_stat = FINDTS(_tag, _tree, _node, _revert);
	if (_stat) {
           if (_logging_level ge 1) { _dummy = fs_logging(_logging_level,_tag,_tree); }
	   return (_node);
	}


        /* Special Case - Confinement via TRANSPORT tree */
        _stat=TreeShr->TreeOpen(ref("TRANSPORT\0"),val(_shotcheck));
        _close="TRANSPORT\0";
        if (_stat) {
           _tree='';
           _stat=FINDSIGTAG(_tag, _tree, _node, _revert);
        }
        if (_closetree) {
           _dummy = TreeShr->TreeClose(ref(_close),val(_shotcheck)); 
        }
	if (_stat) {
           if (_logging_level ge 1) { _dummy = fs_logging(_logging_level,_tag,_tree); }
	   return (_node);
  	}


	/* Default - Search the D3D tree and its sub-trees */
	_stat=TreeShr->TreeOpen(ref("D3D\0"),val(_shotcheck));
	_close="D3D\0";
	if (_stat) {
           _tree='';
	   _stat=FINDSIGTAG(_tag, _tree, _node, _revert);
	}
        if (_closetree) {
           _dummy = TreeShr->TreeClose(ref(_close),val(_shotcheck)); 
        }


        /* Special Case - EFIT pseudo pointnames via runtag */ 
	if ( not(_stat) ) {
	   _stat = FINDEFIT(_tag, _tree);
	   if (_stat) {
	      _stat=TreeShr->TreeOpen(ref(_tree//"\0"),val(_shotcheck));
	      _close=_tree;
              if (_stat) {
	         _stat=FINDSIGTAG(_tag, _tree, _node, _revert);
              }
	   } 
        } 


        /* Default - EFIT via the EFIT01 tree */
	if (not(_stat) && (_tree eq "") ) {
	   _stat = TreeShr->TreeOpen(ref("EFIT01\0"),val(_shotcheck));
	   if (_stat) {
	      _stat=FINDSIGTAG(_tag, _tree, _node, _revert);
           }
           if (_closetree == 1) {
	      _dummy = TreeShr->TreeClose(ref("EFIT01\0"),val(_shotcheck)); 
           }
	}


        /* Compatability - this tree check doesn't really do anything meaningful, but removing it
                           and the _no_ptdata optional argument would cause unnecessary headaches  */
	if (_stat) { 
	   if ((_tree eq "PTDATA") && (_no_ptdata)) {
	      abort(); 
	   } else {
              if (_logging_level ge 1) { _dummy = fs_logging(_logging_level,_tag,_tree); }
	      return(_node);
	   }
	} else {
           if (_logging_level eq 1) { _dummy = fs_logging(_logging_level,_tag,"UNIDENTIFIED"); }
	   abort();
	}

}
