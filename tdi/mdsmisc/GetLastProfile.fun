fun public GetLastProfile(as_is _node)
{
    _nid=getnci(_node,"NID_NUMBER");
    return(MdsMisc->getLastProfile:DSC(_nid));
}