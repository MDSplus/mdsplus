
PUBLIC FUN TREE_FROM_TAG (IN _tag)

/* This function returns the treename contained in a tag \tree::node

        - if the tag is of the form \node, it returns $expt

   Created: 98.02.05  Jeff Schachter
*/

{
	_p=INDEX(_tag,"::"); 


	IF (_p GT 1) {
	  _tree=EXTRACT(1,_p-1,_tag); 
	  RETURN(_tree);
	} ELSE {
	  RETURN($EXPT);
	}

}
