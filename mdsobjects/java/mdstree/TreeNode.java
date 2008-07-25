package mdstree;
import mdsdata.*;


public class TreeNode
{
    int nid;
    
    public TreeNode(int nid)
    {
        this.nid = nid;
        
    }
    public void setNid(int nid)
    {
        this.nid = nid;
    }
    public int getNid()
    {
        return nid;
    }
    public String getPath() throws TreeException
    {
        return MdsTree.getPath(nid);
    }
    public String getFullPath() throws TreeException
    {
        return MdsTree.getFullPath(nid);
    }
    
    public Data getData() throws TreeException
    {
        return MdsTree.getData(nid);
    }
    
    public void putData(Data data) throws TreeException
    {
        MdsTree.putData(nid, data);
    }

    public void deleteData() throws TreeException
    {
        MdsTree.deleteData(nid);
    }
    
    public boolean isOn() throws TreeException
    {
        return MdsTree.isOn(nid);
    }
    
    public void turnOn() throws TreeException
    {
        MdsTree.turnOn(nid);
    }
    public void turnOff() throws TreeException
    {
        MdsTree.turnOff(nid);
    }
    public String getInsertionDate() throws TreeException
    {
        return MdsTree.getInsertionDate(nid);
    }
    
    public String toString()
    {
        try {
            return getPath();
        }catch(Exception exc) {return "";}
    }
}