
PUBLIC FUN CREATE_EFITRT_TEST (IN _shot)
{

	_cmd = "mdstcl @/f/mdsplus/tree/efitrt/create_efitrt_tree_new.tcl '"//ADJUSTL(_shot)//"'";
	write (*,_cmd);
	SPAWN(_cmd);
	RETURN(1);

}

