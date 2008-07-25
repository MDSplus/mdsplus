package mdstree;
import mdsdata.*;

public class CachedTreeNode extends SegmentedTreeNode
{
    public CachedTreeNode(int nid)
    {
        super(nid);
    }

    public void acceptSegment(ArrayData data, Data start, Data end, Data dimension) throws TreeException
    {
        MdsTree.beginCachedSegment(nid, data, start, end, dimension);
    }
    
    public void acceptRow(Data data, long time, boolean isLast) throws TreeException
    {
        MdsTree.putCachedRow(nid, data, time, isLast);
    }

}
