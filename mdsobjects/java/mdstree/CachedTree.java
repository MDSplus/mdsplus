package mdstree;

public class CachedTree extends Tree
{
    boolean isShared;
    int cacheSize;
    public CachedTree(String experiment, int shot)
    {
        super(experiment, shot);
    }
    
    public void open() throws TreeException
    {
        MdsTree.configureCache(isShared, cacheSize);
        MdsTree.openCached(experiment, shot);
    }
    public void close() throws TreeException
    {
        MdsTree.closeCached(experiment, shot);
    }
    
    public void configure(boolean isShared, int cacheSize)
    {
        this.isShared = isShared;
        this.cacheSize = cacheSize;
    }
    
    public void synch()
    {
        MdsTree.synchCache();
    }
    
    public CachedTreeNode getCachedNode(String path) throws TreeException
    {
        int nid = MdsTree.find(path);
        return new CachedTreeNode(nid);
    }
    
    public ActiveTreeNode getActiveNode(String path) throws TreeException
    {
        int nid = MdsTree.find(path);
        return new ActiveTreeNode(nid);
    }
    
}
