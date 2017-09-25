/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
import MDSplus.*;


public class TestMdsObjects 
{
   static void testRows()
   {
       try {

           Connection conn = new Connection("localhost");
           Data ris = conn.get("['oggi', 'piove']");
           System.out.println(ris);
           if(true) System.exit(0);

            Tree tree = new Tree("test",1);
            TreeNode sig=tree.getNode("time");
            Data d = sig.data();
            int[] shape = d.getShape();
            System.exit(0);
            
            sig.deleteData();
            sig.beginTimestampedSegment(new Int8Array(new byte[]{0,0,0,0}));
            sig.putTimestampedSegment(new Int8Array(new byte[]{1,2,3,4}), new long[]{1,2,3,4});
            System.exit(0);
            
            
            
            
            
            
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
           System.exit(0);
       }
       System.out.println("FINITO");
   }
   
   /* This test uses the cached version: it writes data in local cache and flushes it to disk at the end
    * This may improve performence when storing many small rows. When storing large rows, the advantages of 
    * memory cache are lost (in any case we have large I/O on disk and there is the risk of filling memory */
   static void testRowsMem()
   {
       try {
            Tree tree = new Tree("test",1);
            TreeNode sig=tree.getNode("\\sig_1");
            sig.deleteData();
            float []data = new float[10]; //Use a small row
            for(int i = 1; i <=4020; i++)
            {
		for(int j=0;j<10;j++)
                    data[j] = i*10000 + j;
                //if(i < 4020 - 1)
                    sig.putRow(new Float32Array(data), (long)i);
//The last time it is necessary to call putLastRow method                
            }
//This method must be called before exiting in order to allow all pending data to be written on disk            
       }catch(Exception exc)
       {
           System.err.println("Error in testRow: " + exc);
       }
       System.out.println("FINITO");
   }
   
   static void testConnection()
   {
       try {
        Connection c = new Connection("localhost");
        c.openTree("test", -1);
        PutMany p = c.putMany();
        p.append("\\data1", "999999", new Data[0]);
        p.append("\\data2",  "$1*$2", new Data[]{new Float32Array(new float[]{1,2,3,4,5}), new Int32(100)});
        p.execute();
        p.checkStatus("\\data1");
        p.checkStatus("\\data2");
        System.out.println("PUT OK");
        System.exit(0);
        
        
        
        GetMany g = c.getMany();
        g.append("cacca", "2+3", new Data[0]);
        g.append("pipi",  "$1+$2", new Data[]{new Float32Array(new float[]{1,2,3,4,5}), new Int32(100)});
        g.execute();
        Data r = g.get("cacca");
        System.out.println(r);
        r = g.get("pipi");
        System.out.println(r);
        System.exit(0);
        
        
        c.openTree("test", -1);
        c.put("\\data1", "1212121244", new Data[0]);
        c.put("\\data2", "$1+$2", new Data[]{new Float32Array(new float[]{1,2,3,4,5}), new Int32(100)});
        Data d = c.get("$1+$2", new Data[]{new Float32Array(new float[]{1,2,3,4,5}), new Int32(100)});
        System.out.println(d);
       }catch(Exception exc)
       {
           System.out.println(exc);
       }
   }
   
    public static void main(java.lang.String args[])
    {
        try {
            Tree t = new Tree("test", -1);
            TreeNodeArray tna = t.getNodeWild("***");
            java.lang.String str[] = tna.getFullPath();
            for(int i = 0; i < str.length; i++)
                System.out.println(str[i]);
            
        }catch(MdsException exc) 
        {
            System.out.println(exc);
        }

    }
    
    
    
    
    
    
    
    
    
    
    
    
}
