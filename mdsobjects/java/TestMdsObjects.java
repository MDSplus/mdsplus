import MDSplus.*;


public class TestMdsObjects 
{
   static class MyEvent extends Event {
       java.lang.String message;
        public MyEvent(java.lang.String name, java.lang.String message)throws MdsException 
        {
            super(name);
            this.message = message;
        }
        public void run()
        {
            System.out.println("Ricevuto Evento "+ getData() + "  " + message + " " + getTime().getDate());
        }
    }
   
   static void testRows()
   {
       try {
            Tree tree = new Tree("test",1);
            TreeNode sig=tree.getNode("\\sig_1");
            sig.deleteData();
            long startsec = System.currentTimeMillis();
            float []data = new float[1000];
            for(int i = 1; i <=4020; i++)
            {
		for(int j=0;j<1000;j++)
                    data[j] = i*10000 + j;
                sig.putRow(new Float32Array(data), (long)i);
            }
       }catch(Exception exc)
       {
           System.err.println("Error in testRow: " + exc);
       }
       System.out.println("FINITO");
   }
   
   /* This test uses the cached version: it writes data in local cache and flushes it to disk at the end
    * This may improve performence when storing many small rows. When storing large rows, the advantages of 
    * memory cache are lost (in any case we have large I/O on disk and there is the risk of filling memory */
   static void testRowsMem()
   {
       try {
            CachedTree tree = new CachedTree("test",1);
            CachedTreeNode sig=tree.getCachedNode("\\sig_1");
            sig.deleteData();
            float []data = new float[10]; //Use a small row
            for(int i = 1; i <=4020; i++)
            {
		for(int j=0;j<10;j++)
                    data[j] = i*10000 + j;
                if(i < 4020 - 1)
                    sig.putRow(new Float32Array(data), (long)i);
                else
                    sig.putLastRow(new Float32Array(data), (long)i);
            }
            tree.synch();
       }catch(Exception exc)
       {
           System.err.println("Error in testRow: " + exc);
       }
       System.out.println("FINITO");
   }
   
   
   
    public static void main(java.lang.String args[])
    {
         try {
           // Function f = new Function(38, new Data[]{new Int32(2), new Int32(3)});
          //System.out.println(f);
           /*Tree t = new Tree("test", 1, "EDIT");
           t.addNode("DATA1", "ANY");
           t.write();
           if(true) return;*/
           testRowsMem();
           if(true) return;
            
           Event event = new MyEvent("CACCA", "PIPPO");
           //Event event1 = new MyEvent("CACCA", "PLUTO");
           //Event event2 = new MyEvent("CACCA", "TOPOLINO");
           (new Thread(){ 
               public void run()
               {
                   for(int i = 0; i < 10000; i++)
                   {
                       Event.setevent("CACCA", new Int32(i));
                         try {
                            Thread.currentThread().sleep(10);
                        }catch(InterruptedException exc){}
                   }
               }
           }).start();
            
            try {
                Thread.currentThread().sleep(1000);
            }catch(InterruptedException exc){}
           System.out.println("*****************");
           event.dispose();
           event = new MyEvent("CACCA", "PLUTO");
            try {
                Thread.currentThread().sleep(1000);
            }catch(InterruptedException exc){}
           
           System.out.println("*****************");
           event.dispose();
           event = new MyEvent("CACCA", "TOPOLINO");
            try {
                Thread.currentThread().sleep(1000);
            }catch(InterruptedException exc){}
           
           if(true)
               return;
           
           
           
           
           Tree t = new Tree("test", 1);
           TreeNode n = t.getNode("DATA1");
           n.deleteData();
           
           Data apd = new Apd(new Data[]{new Int32(123), new MDSplus.String("CACCA")});
           n.putData(apd);
           if(true) return;
           
           
           
           Data initData = Data.compile("zero(1000, 0.)");
           for(int i = 0; i < 1000; i++)
           {
              Float32 currData = new Float32((float)Math.sin(i/100.));
              n.putRow(currData, (long)i);
           }
           if(true) return;
           
           
           
              
            
            
            
            short raw[] = new short[]{11,22,33,44};
            float []dim = new float[]{1,2,3,4, 5, 6};
            Int16Array i16Arr = new Int16Array(raw, new int[]{2,2});
            Float32Array f32Arr = new Float32Array(dim, new int[]{3,2});
            int [] shape = i16Arr.getShape();
            System.out.println(i16Arr);
            System.out.println(f32Arr);
            System.out.println(i16Arr.getElementAt(new int[]{1}));
            System.out.println(f32Arr.getElementAt(new int[]{2}));
            
            Data d = Data.compile("BUILD_SIGNAL(2*$VALUE, $1, $2)", 
                    new Data[]{new Int16Array(raw), new Float32Array(dim)});
            d.setHelp(new MDSplus.String("CICCIOBOMBO"));
            Data c = new Apd(new Data[]{d, new MDSplus.String("CACCA")});
            //Data d = Data.compile("2+3");
            System.out.println(c);
             
            Tree testTree = new Tree("test", 1);
            TreeNodeArray nodes1 = testTree.getNodeWild("***");
            java.lang.String [] names = nodes1.getFullPath();
            for(int i = 0; i < names.length; i++)
                System.out.println(names[i]);
            
            if(true) return;
            
            
            
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
