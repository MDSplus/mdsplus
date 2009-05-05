import MDSplus.*;


public class TestMdsObjects 
{
    public static void main(java.lang.String args[])
    {
        try {
            
            Tree tree = new Tree("trends", -1, "NEW");
            TreeNode top = tree.getDefault();
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
