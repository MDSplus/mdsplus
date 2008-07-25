package mdstree;
import mdsdata.*;
import java.util.*;

public class ActiveTreeNode extends TreeNode implements DataStreamConsumer, DataStreamProducer
{
    Vector listenersV = new Vector();
    public ActiveTreeNode(int nid) 
    {
        super(nid);
    }
    
    public void putData(Data data) throws TreeException
    {
        MdsTree.putCachedData(nid, data);
    }
    
    public Data getData() throws TreeException
    {
        return MdsTree.getCachedData(nid);
    }
    public void addDataStreamConsumer(DataStreamConsumer consumer) throws TreeException
    {
        if(listenersV.isEmpty())
            MdsTree.registerCallback(nid, this);
        listenersV.addElement(consumer);
    }
    
    public void removeDataStreamConsumer(DataStreamConsumer consumer) throws TreeException
    {
        listenersV.remove(consumer);
        if(listenersV.isEmpty())
            MdsTree.unregisterCallback(nid, this);
    }
    
    public void callback()
    {
        try {
            Data data = getData();
            if(!(data instanceof ApdData))
            {
                System.out.println("Unexpected data in ActiveNode: not an APD");
                return;
            }
            ApdData apd = (ApdData)data;
            Data []descs = apd.getDescArray();
            if(descs.length == 4)
            {
                for(int i = 0; i < listenersV.size(); i++)
                {
                    DataStreamConsumer cons = (DataStreamConsumer)listenersV.elementAt(i);
                    cons.acceptSegment((ArrayData)descs[0], descs[1], descs[2], descs[3]);
                }
            }
            else if(descs.length == 3)
            {
                for(int i = 0; i < listenersV.size(); i++)
                {
                    DataStreamConsumer cons = (DataStreamConsumer)listenersV.elementAt(i);
                    cons.acceptRow(descs[0], descs[1].getLong(), descs[2].getByte() == 1);
                }
            }
            else
            {
                System.out.println("Unexpected number of APD descriptors in ActiveNode");
                return;
            }
        }catch(Exception exc)
        {
            System.err.println("Error in ActiveNodeCallback: " + exc);
        }
    }
    
    public void acceptSegment(ArrayData data, Data start, Data end, Data dimension) throws TreeException
    {
        Data []descs = new Data[4];
        descs[0] = data;
        descs[1] = start;
        descs[2] = end;
        descs[3] = dimension;
        
        ApdData apd = new ApdData(descs);
        putData(apd);
    }
    
    public void acceptRow(Data data, long time, boolean isLast) throws TreeException
    {
        Data []descs = new Data[3];
        descs[0] = data;
        descs[1] = new LongData(time);
        byte last = (isLast)?(byte)1:(byte)0;
        descs[2] = new ByteData(last);
        
        ApdData apd = new ApdData(descs);
        putData(apd);
    }
}
