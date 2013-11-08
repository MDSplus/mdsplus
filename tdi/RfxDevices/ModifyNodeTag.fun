public fun ModifyNodeTag(in _exp , in _shot, in _node, optional in _newNodeName, optional  _RemoveTagName, optional  _AddTagName)
{


	_status = tcl("edit "//_exp//"/shot="//_shot);
    if( !( _status & 1) )
	{
            write(*, "ERRORE cannot open "//_exp//" experiment  "//_shot//" shot");
            return (0);
	}

    _nid = if_error( getnci( build_path( _node ), "NID_NUMBER"), -1);

    if( _nid == -1 )
    {
        write(*, "ERRORE node "//_node//" not found ");
        return (0);
    }
        
    _nodePath = getnci( _nid, "FULLPATH");

    if( present( _newNodeName ) )
    {
        _status = TreeShr->TreeRenameNode( val( _nid ), _newNodeName );
        if( !( _status & 1) )
        {
            write(*, "ERRORE rename "//_nodePath//" node "//getmsg(_status));
            return (0);
        }
    }

    if( present( _RemoveTagName ) )
    {
        _status = TreeShr->TreeRemoveTag( _RemoveTagName );
        if( !( _status & 1) )
    	{
            write(*, "ERRORE remove "//_nodePath//" tag "//getmsg(_status));
            return (0);
    	}
    }

    if( present( _AddTagName ) )
    {
        _status = TreeShr->TreeAddTag( val( _nid ), _AddTagName );
        if( !( _status & 1) )
    	{
            write(*, "ERRORE add "//_nodePath//" tag "//getmsg(_status));
            return (0);
    	}
    }

	_status = tcl("write");
    if( !( _status & 1) )
	{
            write(*, "ERRORE cannot write "//_exp//" experiment  "//_shot//" shot");
            return (0);
	}

    tcl("close");

    return (1);

}
