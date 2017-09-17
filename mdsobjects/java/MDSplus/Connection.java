package MDSplus;
import java.util.*;

public class Connection 
{
    boolean isConnected = false;
    static {
        try {
          System.loadLibrary("JavaMds");
        }catch(Throwable e)
        {
              System.out.println("Error loading library javamds: "+e);
              e.printStackTrace();
        }
    }
    int sockId;
    java.lang.String mdsipAddr;
    boolean checkArgs(Data args[])
    {
        for(int i = 0; i < args.length; i++)
            if(!(args[i] instanceof Scalar) && !(args[i] instanceof Array))
                return false;
        return true;
    }
    
    public Connection(){}
   
    
    public Connection(java.lang.String mdsipAddr) throws MdsException
    {
        initialize(mdsipAddr);
    }
   
    public void initialize(java.lang.String mdsipAddr) throws MdsException
    {
        this.mdsipAddr = mdsipAddr;
        sockId = connectToMds(mdsipAddr);
        if(sockId < 0)
            throw new MdsException("Cannot connect to "+ mdsipAddr);
        isConnected = true;
    }
           
    
    
    protected void finalize()
    {
        if(sockId >= 0)
            disconnectFromMds(sockId);
        isConnected = false;
    }
    
    public void openTree(java.lang.String name, int shot) throws MdsException
    {
        openTree(sockId, name, shot);
    }
            
    public void closeAllTree() throws MdsException
    {
        closeTree(sockId);
    }
    
    public void closeTree(java.lang.String name, int shot) throws MdsException
    {
        closeTree(sockId);
    }
    
    public void setDefault(java.lang.String path) throws MdsException
    {
        setDefault(sockId, path);
    }
    
    public Data get(java.lang.String expr, Data args[]) throws MdsException
    {
        if(!checkArgs(args))
            throw new MdsException("Invalid arguments: only scalars and arrays arguments can be passed to COnnection.get()");
        return get(sockId, expr, args);
    }
    
    public Data get(java.lang.String expr)throws MdsException
    {
        return get(expr, new Data[0]);
    }
    
    public void put(java.lang.String path, java.lang.String expr, Data args[]) throws MdsException
    {
        if(!checkArgs(args))
            throw new MdsException("Invalid arguments: only scalars and arrays arguments can be passed to COnnection.put()");
        put(sockId, path, expr, args);
    }
    public void put(java.lang.String path, java.lang.String expr) throws MdsException
    {
        put(path, expr, new Data[0]);
    }
    public void put(java.lang.String path, Data data) throws MdsException
    {
        put(path, "$", new Data[]{data});
    }
    native int connectToMds(java.lang.String addr);
    native void disconnectFromMds(int sockId);
    public native void openTree(int sockId, java.lang.String name, int shot) throws MdsException;
    public native void closeTree(int sockId) throws MdsException;
    public native void setDefault(int sockId, java.lang.String path) throws MdsException;
    public native Data get(int sockId, java.lang.String expr, Data args[]) throws MdsException;
    public native void put(int sockId, java.lang.String path, java.lang.String expr, Data args[]) throws MdsException;
 
    public GetMany getMany()
    {
        return new GetManyInConnection();
    }
    public PutMany putMany()
    {
        return new PutManyInConnection();
    }
    //////////GetMany 
    class GetManyInConnection extends List implements GetMany
    {
        Dictionary evalRes;
        public void insert(int idx, java.lang.String name, java.lang.String expr, Data [] args)
        {
            Dictionary dict = new Dictionary();
            dict.setItem(new String("exp"), new String(expr));
            dict.setItem(new String("name"), new String(name));
            dict.setItem(new String("args"), new List(args));
            try {
                insert(idx, dict);
            }catch(Exception exc)
            {
                System.out.println("INTERNAL ERROR: GetMany insertion outsize List limits");
            }
        }
        public void append(java.lang.String name, java.lang.String expr, Data [] args)
        {
            insert(len(), name, expr, args);
        }
        
        public void insert(java.lang.String prevName, java.lang.String name, java.lang.String expr, Data [] args)
        {
            int idx;
            for(idx = 0; idx < len() && !getElementAt(idx).equals(new String(prevName)); idx++);
            insert(idx, name, expr, args);
        }
        
        public void remove(java.lang.String name)
        {
            String nameStr = new String(name);
            String nameKeyStr = new String("name");
            for(int i = 0; i < len(); i++)
            {
                Dictionary currDict = (Dictionary)getElementAt(i);
                if(currDict.getItem(nameKeyStr).equals(nameStr))
                {
                    remove(i);
                    return;
                }
            }
        }
        
        public void execute() throws MdsException
        {
            Data serializedIn = new Uint8Array(serialize());
            Data serializedOut = Connection.this.get("GetManyExecute($)", new Data[]{serializedIn});
            evalRes = (Dictionary)Data.deserialize(serializedOut.getByteArray());
        }
        
        public Data get(java.lang.String name) throws MdsException
        {
            if(evalRes == null)
                throw new MdsException("GetMany expressions not evaluated yet");
           Dictionary currDict = (Dictionary)evalRes.getItem(new String(name));
           Data retData = currDict.getItem(new String("value"));
           if(retData == null)
               throw new MdsException(currDict.getItem(new String("error")).getString());
           return retData;
        }
    }
    
    
    class PutManyInConnection extends List implements PutMany
    {
        Dictionary evalRes;
        public void insert(int idx, java.lang.String path, java.lang.String expr, Data [] args)
        {
            Dictionary dict = new Dictionary();
            dict.setItem(new String("exp"), new String(expr));
            dict.setItem(new String("node"), new String(path));
            dict.setItem(new String("args"), new List(args));
            try {
                insert(idx, dict);
            }catch(Exception exc)
            {
                System.out.println("INTERNAL ERROR: GetMany insertion outsize List limits");
            }
        }
        public void append(java.lang.String path, java.lang.String expr, Data [] args)
        {
            insert(len(), path, expr, args);
        }
        
        public void insert(java.lang.String prevPath, java.lang.String path, java.lang.String expr, Data [] args)
        {
            int idx;
            for(idx = 0; idx < len() && !getElementAt(idx).equals(new String(prevPath)); idx++);
            insert(idx, path, expr, args);
        }
        
        public void remove(java.lang.String path)
        {
            String pathStr = new String(path);
            String nodeKeyStr = new String("node");
            for(int i = 0; i < len(); i++)
            {
                Dictionary currDict = (Dictionary)getElementAt(i);
                if(currDict.getItem(nodeKeyStr).equals(pathStr))
                {
                    remove(i);
                    return;
                }
            }
        }
        
        public void execute() throws MdsException
        {
            Data serializedIn = new Uint8Array(serialize());
            Data serializedOut = Connection.this.get("PutManyExecute($)", new Data[]{serializedIn});
            evalRes = (Dictionary)Data.deserialize(serializedOut.getByteArray());
        }
        
        public void checkStatus(java.lang.String path) throws MdsException
        {
            if(evalRes == null)
                throw new MdsException("PutMany not executed yet");
           String retMsg = (String)evalRes.getItem(new String(path));
           if(!retMsg.equals(new String("Success")))
               throw new MdsException(retMsg.getString());
        }
    }
    
///DataStream management
    Hashtable listenerH = new Hashtable();
    Hashtable listenerIdH = new Hashtable();
    Thread listenerThread;
    public synchronized void registerStreamListener(DataStreamListener listener, java.lang.String expr, java.lang.String tree, int shot) throws MdsException
    {
        Data idData = get("MdsObjectsCppShr->registerListener(\""+expr+"\",\""+tree+"\",val("+shot+"))");
        listenerH.put(new Integer(idData.getInt()), listener);
        listenerIdH.put(listener, new Integer(idData.getInt()));
    }
    public synchronized void unregisterStreamListener(DataStreamListener l)
    {
        Integer idInt = (Integer)listenerIdH.get(l);
        if(idInt == null) return;
        int id = idInt.intValue();
        try {
            get("MdsObjectsCppShr->unregisterListener(val("+id+"))");
        }catch(Exception exc) 
        {
            System.out.println(exc);
        }
    }
    
    public void startStreaming()
    {
        if(listenerThread == null)
        {
            listenerThread = new Thread() {
                public void run()
                {
                    checkDataAvailability();
                }
            };
            listenerThread.start();
        }
    }
    
    public void resetConnection()
    {
        disconnectFromMds(sockId);
        sockId = connectToMds(mdsipAddr);
    }
    
    public void disconnect()
    {
        disconnectFromMds(sockId);
        isConnected = false;
    }
    
    void checkDataAvailability()
    {
       while(true)
       {
            try {
                Data serData = get("MdsObjectsCppShr->getNewSamplesSerializedXd:DSC()");
                if(!isConnected) return;  
                Apd apdData = (Apd)Data.deserialize(serData.getByteArray());
                Data [] descs = apdData.getDescs();
                for(int i = 0; i < descs.length/2; i++)
                {
                    int id = descs[2*i].getInt();
                    Signal sig = (Signal)descs[2*i+1];
                    DataStreamListener listener = (DataStreamListener)listenerH.get(new Integer(id));
                    if(listener != null)
                        listener.dataReceived(sig.getData(), sig.getDimensionAt(0));
                }
            }catch(MdsException exc)
            {
                if(!isConnected) return;
                System.out.println("Error in data stream management: " + exc);
            }
       }
    }

//Test program
    public static void main(java.lang.String args[])
    {
        try {
            Connection conn = new Connection("rat2.rfx.local");
            DataStreamListener l1 = new DataStreamListener()
            {
                public void dataReceived(Data samples, Data times)
                {
                    System.out.println("Listener 1 received data:\nSamples: "+samples+"\nTimes: "+times);
                }
            };
            conn.registerStreamListener(l1, "ADC0.SLOW", "falcon_fast", 1);
            conn.startStreaming();
 //           conn.resetConnection();
        }catch(Exception e){System.out.println(e); }
    }
}
    