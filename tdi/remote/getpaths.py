def getpaths(tree):
    """Server side support of getpaths(host,tree) to fetch tree path definitions from data
       server."""
    from MDSplus import Tree,Data,StringArray
    import os
    import socket

    def fixpath(path):
        plist=path.split(';')
        if len(plist) > 1:
            for i in range(len(plist)):
                plist[i]=fixpath(plist[i])
            path=';'.join(plist)
        else:
            parts=path.split(':')
            if len(parts) == 1:
                path="%s::%s" % (socket.gethostname(),path)
            else:
                parts[0]=socket.gethostbyaddr(parts[0])[0]
                path=':'.join(parts)
        return path

    trees=[]
    paths=[]
    try:
        t=Tree(tree,-1)
        treenames=t.findTags('top')
        for treename in treenames:
            try:
                path=os.environ[treename[1:-5].lower()+'_path']
                trees.append(treename[1:-5])
                paths.append(fixpath(path))
            except:
                pass
    except:
        pass
    ans={'tree':StringArray(trees),'path':StringArray(paths)}
    return Data.makeData(ans).serialize()
