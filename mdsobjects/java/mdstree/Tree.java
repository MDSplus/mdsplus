package mdstree;
import mdsdata.*;


public class Tree {
    public static int  TreeUSAGE_ANY  =     0;
    public static int  TreeUSAGE_NONE  =    1; /******* USAGE_NONE is now USAGE_STRUCTURE *******/
    public static int  TreeUSAGE_STRUCTURE =1;
    public static int  TreeUSAGE_ACTION   = 2;
    public static int  TreeUSAGE_DEVICE   = 3;
    public static int  TreeUSAGE_DISPATCH = 4;
    public static int  TreeUSAGE_NUMERIC  = 5;
    public static int  TreeUSAGE_SIGNAL   = 6;
    public static int  TreeUSAGE_TASK     = 7;
    public static int  TreeUSAGE_TEXT     = 8;
    public static int  TreeUSAGE_WINDOW   = 9;
    public static int  TreeUSAGE_AXIS     =10;
    public static int  TreeUSAGE_SUBTREE  =11;
    public static int  TreeUSAGE_COMPOUND_DATA =12;
    String experiment;
    Data data = new Data();//Just to load library
    int shot;
    boolean opened = false;
    
    public Tree(String experiment, int shot)
    {
        this.experiment = experiment;
        this.shot = shot;
    }

    String getExperiment() { return experiment;}
    int getShot() { return shot;}
    
    public void open() throws TreeException
    {
        MdsTree.open(experiment, shot);
        opened = true;
    }

    public void close() throws TreeException
    {
        MdsTree.close(experiment, shot);
        opened = false;
    }
    
    public TreeNode getNode(String path) throws TreeException
    {
        int nid = MdsTree.find(path);
        return new TreeNode(nid);
    }

    public SegmentedTreeNode getSegmentedNode(String path) throws TreeException
    {
        int nid = MdsTree.find(path);
        return new SegmentedTreeNode(nid);
    }

    public TreeNode getParent(int nid) throws TreeException
    {
        int parentNid = MdsTree.getParent(nid);
        return new TreeNode(parentNid);
    }

     
    public TreeNode[] getNodeWild(String path, int usage) throws TreeException
    {
        int []nids = MdsTree.findWild(path, usage);
        TreeNode []nodes = new TreeNode[nids.length];
        for(int i = 0; i < nids.length; i++)
            nodes[i] = new TreeNode(nids[i]);
        return nodes;
    }
    
    public TreeNode[] getNodeWild(String path)  throws TreeException
    {
        return getNodeWild(path, TreeUSAGE_ANY);
    }
    
    public TreeNode [] getMembersOf(TreeNode node) throws TreeException
    {
        int []nids = MdsTree.getMembersOf(node.getNid());
        TreeNode []nodes = new TreeNode[nids.length];
        for(int i = 0; i < nids.length; i++)
            nodes[i] = new TreeNode(nids[i]);
        return nodes;
    }
   
    public TreeNode [] getChildrenOf(TreeNode node) throws TreeException
    {
        int []nids = MdsTree.getChildrenOf(node.getNid());
        TreeNode []nodes = new TreeNode[nids.length];
        for(int i = 0; i < nids.length; i++)
            nodes[i] = new TreeNode(nids[i]);
        return nodes;
    }
   
    public void setDefault(TreeNode node) throws TreeException
    {
        MdsTree.setDefault(node.getNid());
    }
    
    public TreeNode getDefault() throws TreeException
    {
        return new TreeNode(MdsTree.getDefault());
    }
    
    
    
}
