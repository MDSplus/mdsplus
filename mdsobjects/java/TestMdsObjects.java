import MDSplus.*;


public class TestMdsObjects 
{
    public static void main(java.lang.String args[])
    {
        try {
            Tree testTree = new Tree("test", 1);
            TreeNode testNode = testTree.getNode("\\sig_1");
            //Data testData = testNode.getData();
            System.out.println(testNode);
            //Data testData = testNode.getData();
            //System.out.println(testData);
            float[] farr = testNode.getFloatArray();
 /*           for(int i = 0; i < farr.length; i++)
                System.out.println(farr[i]);
            if(true) return;
  */          
            Tree tree = new Tree("trends", -1, "NEW");
            
            System.out.println(Tree.getActiveTree());
            Tree.setActiveTree(testTree);
            System.out.println(Tree.getActiveTree());
            
            
            
            
            TreeNode top = tree.getDefault();
            TreeNode top1 = new TreeNode(0);
            //java.lang.String dec = top.decompile();
            TreeNode memnode = top.addNode("member", "NUMERIC");
            memnode.addTag("m1");
            memnode.addTag("m2");
            TreeNode snode = top.addNode("child", "SUBTREE");
            TreeNode hnode = top.addNode("hardware", "STRUCTURE");
            TreeNode devnode = hnode.addDevice("tr10", "tr10");
            TreeNode channode = tree.getNode(".hardware.tr10.channel_01");
            channode.rename(".chan_01");

		//We do not rename nodes, so we haven't implemented Original Part name method
		//	java.lang.String origName = channode->getOriginalPartName();
		//	printf("%s\n", origName);

            tree.write();

            tree = new Tree("trends", -1);
            TreeNodeArray nodes = tree.getNodeWild("***");
            java.lang.String fullpath[] = nodes.getFullPath();
            for(int i = 0; i < fullpath.length; i++)
                System.out.println(fullpath[i]);

            java.lang.String tags[] = tree.findTags("M");
            for(int i = 0; i < tags.length; i++)
                System.out.println(tags[i]);
            
            TreeNode currNode = tree.getNode("member");
            tags = currNode.findTags();
            for(int i = 0; i < tags.length; i++)
                System.out.println(tags[i]);
        }catch(MdsException exc)
        {
            System.out.println("Error: " + exc);
        }
    }
}
