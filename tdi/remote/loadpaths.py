def loadpaths(host,treename):
    """Get the path environment variables for a tree being served by a remote system and
    define them in this process context. The remote system's getpaths function is responsible
    for fixing up the tree_path values, prefixing hostnames and replacing any shortened hostnames
    with fully qualified ip address names."""

    from MDSplus import Connection
    import os
    c=Connection(host)
    paths = c.get("getpaths('%s')" % treename).deserialize()
    for i in range(len(paths['path'])):
        os.environ[paths['tree'][i].rstrip().lower()+'_path']=paths['path'][i].rstrip()
