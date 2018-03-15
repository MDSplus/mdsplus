
PUBLIC FUN CREATE_EFITRT_TEST (IN _shot)
{

	/* This routine used to create EFITRT test trees on atlas - i.e. 700000+, 900000+ */
        _cmd = "mdstcl @/fusion/usc/src/mds_btshot/efitrt/create_efitrt_tree_new.tcl '"//ADJUSTL(_shot)//"'";
	write (*,_cmd);
	SPAWN(_cmd);
	RETURN(1);
}

