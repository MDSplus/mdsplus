
fun public efittree(IN _tag, OPTIONAL IN _shot, 
	                        OPTIONAL IN _tag_prefix, 
                                OPTIONAL IN _parent, 
                                OPTIONAL IN _parent_path)

/*   Find the subtree node corresponding to the tag name 
 *   _tag_prefix//_tag for subtrees of _parent under _parent_path.
 */

{
  if (NOT PRESENT(_tag)) ABORT();
  if (NOT PRESENT(_shot)) _shot = $shot;

  if (_tag EQ "") ABORT();  /* put in to avoid seg fault somewhere down below in fun if _tag="" */

  /*  REPLACE THE SETTINGS OF _tag_prefix, _parent, AND _parent_path 
   *  WITH VALUES APPROPRIATE FOR YOUR SITE 
   */
  if (NOT PRESENT(_tag_prefix)) _tag_prefix = "\\EFIT_";
  if (NOT PRESENT(_parent)) _parent = "MHD";  
  if (NOT PRESENT(_parent_path)) _parent_path = "\\MHD::TOP.EFIT";

  /*** search parent tree for tag, expecting to find node EFITnn ***/

  _b0 = IACHAR(EXTRACT(4,1,_tag));	
  _b1 = IACHAR(EXTRACT(5,1,_tag));	

  if ( (LEN(_tag) NE 6) OR
       (UPCASE(EXTRACT(0,4,_tag)) NE "EFIT") OR
       (_b0 < 48) OR (_b0 > 57) OR
       (_b1 < 48) OR (_b1 > 57) )
    {
      if (TreeOpen(_parent//",DOESNOTEXIST",_shot))
	{
	  _node = GETNCI(_tag_prefix//_tag, "FULLPATH");
	  TreeClose(_parent, _shot);

	  _l = LEN(_parent_path);
	  _test = EXTRACT(0,_l+5,_node);  /* chop off "\PARENT_PATH.EFIT" */

	  if ( (_test EQ _parent_path//".EFIT") ||
	       (_test EQ _parent_path//":EFIT") )	
	    {
	      _tree = EXTRACT(_l+1,6,_node);  /* extract out EFITnn from \PARENT_PATH.EFITnn */
	      RETURN(_tree);
	    }
	  else ABORT();
	}
      else ABORT();
    }  

    /* _tag is already subtree name EFITnn */
  else RETURN(_tag);
}

      
    
