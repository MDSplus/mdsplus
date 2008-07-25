package mdstree;
import mdsdata.*;
public class SegmentedTreeNode extends TreeNode implements DataStreamConsumer
{
    public SegmentedTreeNode(int nid)
    {
        super(nid);
    }

    public void acceptSegment(ArrayData data, Data start, Data end, Data dimension) throws TreeException
    {
        MdsTree.beginSegment(nid, data, start, end, dimension);
    }
    
    public void acceptRow(Data data, long time, boolean isLast) throws TreeException
    {
        MdsTree.putRow(nid, data, time, isLast);
    }

}
