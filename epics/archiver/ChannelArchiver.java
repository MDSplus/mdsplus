import java.util.*;
import java.util.concurrent.*;
import gov.aps.jca.*;
import gov.aps.jca.dbr.*;
import gov.aps.jca.event.*;
import java.io.*;
import MDSplus.*;


public class ChannelArchiver 
{
    static int SEGMENT_SIZE;
    static int MAX_QUEUE_LEN  = 10000;
    static boolean debug = false;
    static Hashtable monitorInfo = new Hashtable();
    static Hashtable bufferH = new Hashtable(); //Indexed by path name
    static Data DBR2Data(DBR dbr) throws Exception
    {
        Data data = null;
        int count = dbr.getCount();
        if(dbr.isBYTE())
        {
            byte[] val = ((DBR_Byte)dbr).getByteValue();
            if(count > 1)
                data = new Int8Array(val);
            else
                data = new Int8(val[0]);
        }
        else if(dbr.isSHORT())
        {
            short[] val = ((DBR_Short)dbr).getShortValue();
            if(count > 1)
                data = new Int16Array(val);
            else
                data = new Int16(val[0]);
        }
        else if(dbr.isINT())
        {
            int[] val = ((DBR_Int)dbr).getIntValue();
            if(count > 1)
                data = new Int32Array(val);
            else
                data = new Int32(val[0]);
        }
        else if(dbr.isFLOAT())
        {
            float[] val = ((DBR_Float)dbr).getFloatValue();
            if(count > 1)
                data = new Float32Array(val);
            else
                data = new Float32(val[0]);
        }
        else if(dbr.isDOUBLE())
        {
            double[] val = ((DBR_Double)dbr).getDoubleValue();
            if(count > 1)
                data = new Float64Array(val);
            else
                data = new Float64(val[0]);
        }
        else if(dbr.isSTRING())
        {
            java.lang.String[] val = ((DBR_String)dbr).getStringValue();
            if(count > 1)
                data = new StringArray(val);
            else
                data = new MDSplus.String(val[0]);
        }
        else throw new Exception("Unsupported DBR type");
        return data;
    }
    
    static long DBR2Time(DBR dbr) throws Exception
    {
        if(!dbr.isTIME())
            throw new Exception("Time not supported");
            
        if(dbr.isBYTE())
            return (long)(((DBR_TIME_Byte)dbr).getTimeStamp().asDouble()*1E9);
        if(dbr.isSHORT())
            return (long)(((DBR_TIME_Short)dbr).getTimeStamp().asDouble()*1E9);
        if(dbr.isINT())
            return (long)(((DBR_TIME_Int)dbr).getTimeStamp().asDouble()*1E9);
        if(dbr.isFLOAT())
            return (long)(((DBR_TIME_Float)dbr).getTimeStamp().asDouble()*1E9);
        if(dbr.isDOUBLE())
            return (long)(((DBR_TIME_Double)dbr).getTimeStamp().asDouble()*1E9);
        throw new Exception("Unsupported Type in getTimestamp()");
    }

    static DBRType DBR2TimedType(DBR dbr) throws Exception
    {
        if(dbr.isBYTE())
            return DBRType.TIME_BYTE;
        if(dbr.isSHORT())
            return DBRType.TIME_SHORT;
        if(dbr.isINT())
            return DBRType.TIME_INT;
        if(dbr.isFLOAT())
            return DBRType.TIME_FLOAT;
        if(dbr.isDOUBLE())
            return DBRType.TIME_DOUBLE;
        throw new Exception("Unsupported Type in getTimestamp()");
    }

    static int DBR2NItems(DBR dbr) throws Exception
    {
        return dbr.getCount();
    }
    
    static int CAStatus2Severity(CAStatus status)
    {
	CASeverity severity = status.getSeverity();
	if(severity == CASeverity.SUCCESS) return 0;
	if(severity == CASeverity.INFO) return 1;
	if(severity == CASeverity.WARNING) return 2;
	if(severity == CASeverity.ERROR) return 3;
	if(severity == CASeverity.SEVERE) return 4;
	if(severity == CASeverity.FATAL) return 5;
	return 0;
    }
    
            
    static class TreeDataDescriptor
    {
        static final int BYTE = 1, SHORT = 3, INT = 4, LONG = 5, FLOAT = 6, DOUBLE = 7;
    	java.lang.String nodeName;
   	double []doubleVals;
  	float []floatVals;
        int [] intVals;
        byte [] byteVals;
        long []longVals;
        short[] shortVals;
    	long []times;
    	int idx;
        int type;
	int segmentSize;
        boolean isArray;
        Array array;
     	TreeDataDescriptor(java.lang.String nodeName, int segmentSize)
    	{
	    this.segmentSize = segmentSize;
	    doubleVals = new double[segmentSize];
	    floatVals = new float[segmentSize];
	    intVals = new int[segmentSize];
	    longVals = new long[segmentSize];
	    shortVals = new short[segmentSize];
	    byteVals = new byte[segmentSize];
	    times = new long[segmentSize];
	    idx = 0;
	    this.nodeName = nodeName;
    	}
	java.lang.String getNodeName(){return nodeName;}
    	boolean addRow(Data val, long time)
    	{
 	    if(idx < segmentSize)
	    {
               
                try {
                   if(val instanceof Array)
                   {
                       isArray = true;
                       array = (Array)val;
                       return true;
                   }
                   else
                   {
                       isArray = false;
                       if(val instanceof Float64)
                       {
                           type = DOUBLE;
                           doubleVals[idx] = val.getDouble();
                       }
                       else if (val instanceof Float32)
                       {
                           type = FLOAT;
                           floatVals[idx] = val.getFloat();
                       }
                       else if (val instanceof Int64)
                       {
                           type = LONG;
                           longVals[idx] = val.getLong();
                       }
                       else if (val instanceof Int32)
                       {
                           type = INT;
                           intVals[idx] = val.getInt();
                       }
                       else if (val instanceof Int16)
                       {
                           type = SHORT;
                           shortVals[idx] = val.getShort();
                       }
                       else if (val instanceof Int8)
                       {
                           type = BYTE;
                            byteVals[idx] = val.getByte();
                       }
                       else
                       {
                           System.err.println("Unexpected data type");
                           return false;
                       }
                   } 
                   times[idx] = time;
                   idx++;
               }catch(Exception exc) {System.err.println("Internal error in data management");}
            }
            return idx == segmentSize;
    	}
	Array getVals() 
        {
            switch(type) {
                case BYTE: return new Int8Array(byteVals);
                case SHORT: return new Int16Array(shortVals);
                case INT: return new Int32Array(intVals);
                case FLOAT: return new Float32Array(floatVals);
                case DOUBLE: return new Float64Array(doubleVals);
                default: return null;
            }
        }
	Data getVal() 
        {
            if(isArray)
            {
                int[]shape = array.getShape();
                int[]newShape = new int[shape.length + 1];
                for(int i = 0; i < shape.length; i++)
                    newShape[i] = shape[i];
                newShape[shape.length] = 1;
                try {
                    array.setShape(newShape);
                    return array;
                }catch(Exception exc){System.err.println("Cannot reshape Array"); return null;}                
            }
            switch(type) {
                case BYTE: return new Int8(byteVals[0]);
                case SHORT: return new Int16(shortVals[0]);
                case INT: return new Int32(intVals[0]);
                case FLOAT: return new Float32(floatVals[0]);
                case DOUBLE: return new Float64(doubleVals[0]);
                default: return null;
            }
        }
	long [] getTimes() { return times;}
	//int getDim() { return vals.length;}
        int getDim() { return idx;}
     } //End static inner class TreeDataDescriptor
 

   
    //TreeHandler Manages insertion of data into MDSplus tree
    static class TreeHandler implements Runnable
    {
        class SizeChecker implements Runnable
        {
            boolean terminated = false;
            Tree tree;
            SizeChecker(Tree tree)
            {
                this.tree = tree;
            }
            void terminate()
            {
                terminated = true;
            }
            public void run()
            {
		long prevSize = 0;
                //while(!terminated)
                {
                    try {
                        currSize = tree.getDatafileSize();
			if(debug) System.out.println("QUEUE SIZE: " + queue.size());
			if(debug) System.out.println("FILE SIZE: " + currSize);
			if(currSize > 1000000000 && currSize == prevSize)
			    ChannelArchiver.debug = true;
			prevSize = currSize;

                    }catch(Exception exc){System.err.println("Cannot get Datafile Size: " + exc); 
                    //System.exit(0);
                    }
                    try {
                        Thread.currentThread().sleep(5000); //Repeat check every 5 seconds
                    }catch(InterruptedException exc){return;}
                }
             }
        }//End static inner class SizeChecher
	

        long switchSize;
        long currSize;
        SizeChecker sizeChecker;
        int currShot;
        Tree model;
        Tree tree;
	Hashtable treeNodeHash = new Hashtable();
        java.lang.String expName;
        LinkedBlockingQueue<TreeDataDescriptor> queue;
        TreeHandler(java.lang.String expName, Tree model, int shot, long switchSize, LinkedBlockingQueue<TreeDataDescriptor> queue) throws Exception
        {
            this.queue = queue;
            this.expName = expName;
            this.model = model;
            currSize = 0;
            this.switchSize = switchSize;
            tree = new Tree(expName, shot);
            currShot = shot;
            sizeChecker = new SizeChecker(tree);
            (new Thread(sizeChecker)).start();
        }
        
        public void run()
        {
            while(true)
            {
		TreeDataDescriptor descr = null;
                try {
                    descr = queue.take();
		} catch(Exception exc){System.err.println("Error dequeuing request: "+exc); System.exit(0);}
		java.lang.String nodeName = descr.getNodeName();
		TreeNode node = (TreeNode)treeNodeHash.get(nodeName);
		if(node == null)
		{
		    try {
			node = tree.getNode(nodeName);
			treeNodeHash.put(nodeName, node);
		    }catch(Exception exc) 
		    {
			System.err.println("Error getting tree node for " + nodeName + ": " +exc);
		    }
		}
	    	try {
		    if(descr.getDim() >1)
	   	    	node.makeTimestampedSegment(descr.getVals(), descr.getTimes());
		    else
		    {
			node.putRow(descr.getVal(), descr.getTimes()[0]);
		    }
	   	}catch(Exception exc){ System.err.println("Error in TimestampedSegment: " + exc);}
            	if(currSize > switchSize)
            	{
 		    if(debug) System.out.println("REACHED FILE SIZE LIMIT: " + currSize + " " + switchSize);
               	    currShot++;
		    try {
                    	model.createPulse(currShot);
                    	tree = new Tree(expName, currShot);
			treeNodeHash.clear();
                    	sizeChecker.terminate();
               	    	currSize = 0;
                    	sizeChecker = new SizeChecker(tree);
                    	(new Thread(sizeChecker)).start();
		    }catch(Exception exc){System.err.println("Error creating pulse: " + exc);}
             	}
            }
        }
        
    } //End static inner  class TreeHandler
    

    //TreeHandlerConnection Manages insertion of data into MDSplus tree using thin client and GetMany
    static class TreeHandlerConnection implements Runnable
    {
	Connection connection;
        java.lang.String expName;
        LinkedBlockingQueue<TreeDataDescriptor> queue;
        TreeHandlerConnection(java.lang.String ipAddress, java.lang.String expName, int shot,  LinkedBlockingQueue<TreeDataDescriptor> queue) throws Exception
        {
            this.queue = queue;
	    connection = new Connection(ipAddress);
	    connection.openTree(expName, shot);
        }
        
        public void run()
        {
	    long startTime, endTime;
            while(true)
            {
		GetMany getMany = null;
		int count = 0;
		startTime = System.currentTimeMillis();
		while (queue.size() > 0)
		{
		    if(getMany == null)
			getMany = connection.getMany();
			TreeDataDescriptor descr = null;
                    try {
                        descr = queue.take();
		    } catch(Exception exc){System.err.println("Error dequeuing request: "+exc); System.exit(0);}
		   
		    if(descr.getDim() > 1)
		    {
		    	Data args[] = new Data[2];
			args[0] = new Int64Array(descr.getTimes());
			args[1] = descr.getVals();
			getMany.append("put_"+count, "MakeTimestampedSegment("+descr.getNodeName()+",$1,$2)", args);
		    }
		    else
		    {
			getMany.append("put_"+count, "PutRow("+descr.getNodeName()+",1000,"+descr.getTimes()[0]+"Q,"+descr.getVal().decompile()+")", new Data[0]);
		    }
		    count++;
		    if(count > 10000)
		    {
			System.out.println("Warning: more than 10000 pending write operations");
			break;
		    }
		}
		if(getMany != null)
		{
		    try {
			if(debug)
			    System.out.println("Writing "+count+ " data items");
			 getMany.execute();
		    } catch(Exception exc){System.err.println("Error in GetMany.execute(): " + exc);}
             	}
		endTime = System.currentTimeMillis();
		long interval = endTime - startTime;
		if(debug)
		    System.out.println("Pending Write queue length: " + queue.size());
		if(interval < 1000)
		{
		    try {
		    	Thread.currentThread().sleep(1000-interval); 
		    }catch(InterruptedException exc){}
		}
            }
        }
        
    } //End static inner  class TreeHandlerConnection




    static class TreeManager
    {
        TreeHandler treeH;
        Hashtable nodeHash = new Hashtable();
        LinkedBlockingQueue<TreeDataDescriptor> queue;
	Tree model;
        TreeManager(java.lang.String expName, Tree model, int shot, long switchSize) throws Exception
        {
            queue = new LinkedBlockingQueue<TreeDataDescriptor>(MAX_QUEUE_LEN);
            treeH = new TreeHandler(expName, model, shot, switchSize, queue);
	    this.model = model;
            (new Thread(treeH)).start();
        }
	TreeManager(){}
        synchronized void putRow(java.lang.String treeNodeName, Data data, long time, int segmentSize)
        {
	    TreeDataDescriptor descr = (TreeDataDescriptor)nodeHash.get(treeNodeName);
            if(descr == null)
            {
                try {
                    nodeHash.put(treeNodeName, descr = new TreeDataDescriptor(treeNodeName, segmentSize));
                }catch(Exception exc)
                {
                    System.err.println("INTERNAL ERROR: Cannot get node for "+treeNodeName + ": " + exc);
                    return;
                }
            }
            try {
              	if(descr.addRow(data, time))
	    	{
                    nodeHash.put(descr.getNodeName(), new TreeDataDescriptor(descr.getNodeName(), segmentSize));
                    if(queue.remainingCapacity() > 0)
                    {
                        queue.put(descr);
                    }
                    else
                    	System.out.println("WARNING: discarded block for " + descr.getNodeName()); 
	    	}
            }catch(Exception exc){System.err.println("Error enqueuing putRow request");}
        }
        
    }//End inner class TreeManager
    
    static class TreeManagerConnection extends TreeManager
    {
	TreeHandlerConnection treeHC;
	java.lang.String ipAddress;
        TreeManagerConnection(java.lang.String ipAddress, java.lang.String expName, int shot) throws Exception
        {
            queue = new LinkedBlockingQueue<TreeDataDescriptor>(MAX_QUEUE_LEN);
            treeHC = new TreeHandlerConnection(ipAddress, expName,shot, queue);
            (new Thread(treeHC)).start();
        }
        synchronized void putRow(java.lang.String treeNodeName, Data data, long time, int segmentSize)
        {
	    TreeDataDescriptor descr = (TreeDataDescriptor)nodeHash.get(treeNodeName);
            if(descr == null)
            {
                try {
                    nodeHash.put(treeNodeName, descr = new TreeDataDescriptor(treeNodeName, segmentSize));
                }catch(Exception exc)
                {
                    System.err.println("INTERNAL ERROR: Cannot get node for "+treeNodeName + ": " + exc);
                    return;
                }
            }
            try {
              	if(descr.addRow(data, time))
	    	{
                    nodeHash.put(descr.getNodeName(), new TreeDataDescriptor(descr.getNodeName(), segmentSize));
                    if(queue.remainingCapacity() > 0)
                    {
                        queue.put(descr);
                    }
                    else
                    	System.out.println("WARNING: discarded block for " + descr.getNodeName()); 
	    	}
            }catch(Exception exc){System.err.println("Error enqueuing putRow request");}
        }
        
    }//End inner class TreeManagerConnection
    
   
    
    static class DataAndTime
    {
        Data data;
        long time;
        DataAndTime(Data data, long time)
        {
            this.data = data;
            this.time = time;
        }
        final Data getData(){return data;}
        final long getTime() { return time;}
    }//End static class DataAndTime
    
    static class DataMonitor implements MonitorListener
    {
        TreeManager treeManager;
        java.lang.String treeNodeName;
        java.lang.String severityNodeName;
        boolean saveTree;
        Data currData = null;
        long currTime;
	int currSeverity;
        long prevTime = 0;
        int ignFuture;
	int prevSeverity = -1;
	int segmentSize;

        public DataMonitor(TreeManager treeManager, java.lang.String treeNodeName, int segmentSize, java.lang.String severityNodeName, int ignFuture)
        {
            this.treeManager = treeManager;
            this.treeNodeName = treeNodeName;
	    this.segmentSize = segmentSize;
            this.severityNodeName = severityNodeName;
            saveTree = true;
            this.ignFuture = ignFuture;

        }
        public DataMonitor(int ignFuture)
        {
            treeManager = null;
            treeNodeName = null;
            severityNodeName = null;
            saveTree = false;
            this.ignFuture = ignFuture;
        }
 	public synchronized void monitorChanged(MonitorEvent e)
	{
	    Integer currCount = (Integer)monitorInfo.get(treeNodeName);
	    if(currCount == null)
	    {
		currCount = new Integer(1);
		monitorInfo.put(treeNodeName, currCount);
	    }
	    else
	    {
		currCount = new Integer(currCount.intValue()+1);
		monitorInfo.put(treeNodeName, currCount);
	    }
            DBR dbr = e.getDBR();
            try {
                Data data = DBR2Data(dbr);
                long time = DBR2Time(dbr);
		int severity = CAStatus2Severity(e.getStatus());
                if(time <= prevTime)  //A previous sample has been received
                {
                    //System.out.println("---PREVIOUS SAMPLE!!! Time: "+time + " Previous time: " + prevTime);
                    return;
                }
                if(prevTime > 0 && ((time - prevTime)/1E12 > ignFuture)) //Too far in future
		{
                    System.out.println("Too far in future ignFuture " +ignFuture + "(time - prevTime)" + (time - prevTime) );
                    return; 
		}
                prevTime = time;
                 if(saveTree)
                 {
                    treeManager.putRow(treeNodeName, data, time, segmentSize);
		    if(severity != prevSeverity)
		    {
		        treeManager.putRow(severityNodeName, new Int8((byte)severity), time, segmentSize);
	        	prevSeverity = severity;
                   }
                }
                else
                {
                    currData = data;
                    currTime = time;
		    currSeverity = severity;
                }
            }catch(Exception exc)
            {
                System.err.println("Error writing sample: " + exc);
            }	
        }
        public synchronized DataAndTime getDataAndTime()
        {
            return new DataAndTime(currData, currTime);
        }
	public synchronized int getSeverity() { return currSeverity;}
        
    }//End static class 
    
    static class DataScanner implements Runnable
    {
        static final int SCAN = 1, MONITOR = 2;
        int mode;
        java.lang.String treeNodeName;
        java.lang.String severityNodeName;
        TreeManager treeManager;
        Channel chan;
        Context ctxt;
        long period;
        long prevTime = 0;
        int ignFuture;
        DataMonitor monitor;
        DBRType dataType = null;
        int nItems;
	int prevSeverity;
	int segmentSize;
        
        public DataScanner(TreeManager treeManager, java.lang.String treeNodeName, int segmentSize, java.lang.String severityNodeName, Channel chan, Context ctxt, 
                long period, int ignFutur)
        {
            this.treeManager = treeManager;
            this.treeNodeName = treeNodeName;
	    this.segmentSize = segmentSize;
	    this.severityNodeName = severityNodeName;
            this.chan = chan;
            this.ctxt = ctxt;
            this.period = period;
            this.ignFuture = ignFuture;
            mode = SCAN;
            
        }
        public DataScanner(TreeManager treeManager, DataMonitor monitor, java.lang.String treeNodeName, java.lang.String severityNodeName, Channel chan, 
                Context ctxt, long period, int ignFutur)
        {
            this.treeManager = treeManager;
            this.treeNodeName = treeNodeName;
	    this.segmentSize = segmentSize;
	    this.severityNodeName = severityNodeName;
            this.monitor = monitor;
            this.chan = chan;
            this.ctxt = ctxt;
            this.period = period;
            this.ignFuture = ignFuture;
            mode = MONITOR;
	    int prevSeverity = -1;
        }
        public void run()
        {
            while(true)
            {
                long time;
                Data data;
		int severity = -1;
                try {
                    Thread.currentThread().sleep(period);
                } catch(InterruptedException exc){return;}
                try {
                    if(mode == SCAN)
                    {
                        if(dataType == null)
                        {
                            DBR dbr = chan.get();
                            dataType = DBR2TimedType(dbr);
                            nItems = DBR2NItems(dbr);
                        }
                        DBR dbr = chan.get(dataType, nItems);
                        ctxt.pendIO(5.);
                        data = DBR2Data(dbr);
                        time = DBR2Time(dbr);
//!!!!!!!Apparently it is not possible to get severity from object Channel!!!!!!!!!!!
			
                    }
                    else //mode == MONITOR
                    {
                        DataAndTime dataTime = monitor.getDataAndTime();
                        data = dataTime.getData();
                        time = dataTime.getTime();
			severity = monitor.getSeverity();
                    }
                    if(time <= prevTime)  //A previous sample has been received
                        continue;
                    if(prevTime > 0 && ((time - prevTime)/1E9 > ignFuture)) //Too far in future
                        continue; 
                    prevTime = time;
                    
                    treeManager.putRow(treeNodeName, data, time, segmentSize);
		    if(severity != prevSeverity)
		    {
		       treeManager.putRow(severityNodeName, new Int8((byte)severity), time, segmentSize);
		       prevSeverity = severity;
		    }
                }catch(Exception exc)
                {
                    System.err.println("Error writing sample: " + exc);
                }
            }
        }
    }

    static java.lang.String  getPVname(MDSplus.Tree tree, MDSplus.TreeNode node)
    {
	try {
	    int nid = node.getNid();
	    MDSplus.TreeNode currNode = node;
	    while(nid != 0)
	    {
	    	currNode = currNode.getParent();
	    	nid = currNode.getNid();
	    	try {
		    MDSplus.TreeNode bufSizeNode = tree.getNode(currNode.getFullPath()+":REC_NAME");
		    java.lang.String  pvName = bufSizeNode.getData().getString();
		    return pvName;
	    	} catch(Exception exc){}
	     }
	}catch(Exception exc)
	{
	    System.out.println("INTERNAL ERROR in getBPVname(): " + exc);
	}
	return "";
    }

    static int getBufSize(MDSplus.Tree tree, MDSplus.TreeNode node)
    {
	try {
	    int nid = node.getNid();
	    MDSplus.TreeNode currNode = node;
	    while(nid != 0)
	    {
	    	currNode = currNode.getParent();
	    	nid = currNode.getNid();
	    	try {
		    MDSplus.TreeNode bufSizeNode = tree.getNode(currNode.getFullPath()+":BUF_SIZE");
		    int bufSize = bufSizeNode.getInt();
		    return bufSize;
	    	} catch(Exception exc){}
	     }
	}catch(Exception exc)
	{
	    System.out.println("INTERNAL ERROR in getBufferSize(): " + exc);
	}
	return SEGMENT_SIZE;
    }

    public static void main(java.lang.String[] args)
    {

        //Global Parameters
        float getThreshold; //treshold above which use real scan instead of monitor scan
        int ignFuture; //Number of seconds above which skip recording
        long fileSize; //DImension above which create a new pulse;
        Tree tree = null;
        Hashtable groupH = new Hashtable();
	boolean createPulse;

	java.lang.String mdsipAddress = System.getProperty("mdsip");
	java.lang.String debugTxt = System.getProperty("debug");
	debug = (debugTxt != null && debugTxt.equals("yes"));
	java.lang.String createPulseTxt = System.getProperty("create-pulse");
	createPulse = (createPulseTxt != null && createPulseTxt.equals("yes"));


//testMdsAccess();
//if(true) return;

        if(args.length != 2 && args.length != 3)
        {
            System.out.println("Usage:java ChannelArchiver <experiment> <shot> [segment_size]");
            return;
        }
        java.lang.String experiment = args[0];
        int shot = Integer.parseInt(args[1]);
	if(args.length == 3)
	    SEGMENT_SIZE = Integer.parseInt(args[2]);
	else
	    SEGMENT_SIZE = 1;

        try {
            JCALibrary jca = JCALibrary.getInstance();
            Context ctxt = jca.createContext(JCALibrary.CHANNEL_ACCESS_JAVA);
            tree = new Tree(experiment, -1);
	    if(createPulse)
		tree.createPulse(shot);
//Get Global Parameters            
            try {
             TreeNode node = tree.getNode(":GET_TRESH");
             getThreshold = node.getFloat();
            }catch(Exception exc)
            {
                getThreshold = 1;
            }
            try {
             TreeNode node = tree.getNode(":IGN_FUTURE");
             ignFuture = node.getInt()*3600;
            }catch(Exception exc)
            {
                ignFuture = 6*3600;
            }
            try {
             TreeNode node = tree.getNode(":FILE_SIZE");
             fileSize = node.getLong()* 1000000;
	    
            }catch(Exception exc)
            {
                fileSize = 1000000000;
            }
            

            TreeManager treeManager;
	    if(mdsipAddress == null)
		treeManager = new TreeManager(experiment, tree, shot, fileSize);
	    else
		treeManager = new TreeManagerConnection(mdsipAddress, experiment, shot);
            TreeNodeArray treeNodeArr = tree.getNodeWild("***");
            java.lang.String []nodeNames = treeNodeArr.getPath();
            int[] nids = treeNodeArr.getNid();
            for(int i = 0; i < nodeNames.length; i++)
            {
                if(nodeNames[i].endsWith(":REC_NAME"))
                {
                    java.lang.String nodeName = nodeNames[i].substring(0, nodeNames[i].length() - 9);
                    java.lang.String recName  = "";
                    
                    try {
                        recName = new TreeNode(nids[i], tree).getData().getString();
			recName = recName.trim();
                        if(debug) System.out.println("Scanning...."+recName);
                        //Get VAL channel. It will remain open thorough the whole program execution
                        Channel valChan = ctxt.createChannel(recName+".VAL");
                        ctxt.pendIO(5.);
                        DBR valDbr = valChan.get();
                        ctxt.pendIO(5.);
			if(debug) System.out.println("Monitoring Channel created.");
                        if(!valDbr.isENUM() && !valDbr.isCTRL()&&! valDbr.isINT())
                        {
                            //EGU
                            try {
                                Channel eguChan = ctxt.createChannel(recName+".EGU");
                                ctxt.pendIO(5.);
                                DBR dbr = eguChan.get();
                                ctxt.pendIO(5.);
                                TreeNode eguNode = tree.getNode(nodeName+":EGU");
				Data dd = DBR2Data(dbr);
                                eguNode.putData(DBR2Data(dbr));
                                eguChan.destroy();
                             }catch(Exception exc)
                             {
                                 System.err.println("Cannot get EGU for " + recName + ": " + exc);
                                 //continue;
                             }
                            //HOPR
                            try {
                                Channel hoprChan = ctxt.createChannel(recName+".HOPR");
                                ctxt.pendIO(5.);
                                DBR dbr = hoprChan.get();
                                ctxt.pendIO(5.);
                                TreeNode hoprNode = tree.getNode(nodeName+":HOPR");
                                hoprNode.putData(DBR2Data(dbr));
                                hoprChan.destroy();
                             }catch(Exception exc)
                             {
                                 System.err.println("Cannot get HOPR for " + recName + ": " + exc);
                             }
                             //LOPR
                            try {
                                Channel loprChan = ctxt.createChannel(recName+".LOPR");
                                ctxt.pendIO(5.);
                                DBR dbr = loprChan.get();
                                ctxt.pendIO(5.);
                                TreeNode hoprNode = tree.getNode(nodeName+":LOPR");
                                hoprNode.putData(DBR2Data(dbr));
                                loprChan.destroy();
                            }catch(Exception exc)
                            {
                                System.err.println("Cannot get LOPR for " + recName + ": " + exc);
                            }                        
                        }
                        
                        
                        //Get SCAN mode for this channel
                        TreeNode valNode = tree.getNode(nodeName+":VAL");
                        TreeNode scanNode = tree.getNode(nodeName+":SCAN_MODE");
                        TreeNode severityNode = tree.getNode(nodeName+":ALARM");
                        java.lang.String scanMode = scanNode.getString().toUpperCase();
			int segmentSize = getBufSize(tree, valNode);
			if(debug) System.out.println("Monitoring channel started. Segment size: "+segmentSize);
                        if(scanMode.equals("MONITOR"))
                        {
                            if(valDbr.isENUM() || valDbr.isCTRL() || valDbr.isINT())
                               valChan.addMonitor(DBRType.TIME_INT, 1, Monitor.VALUE, 
                               	new DataMonitor(treeManager, valNode.getFullPath(), segmentSize, severityNode.getFullPath(), ignFuture));
                            else
                               valChan.addMonitor(DBRType.TIME_DOUBLE, 1, Monitor.VALUE, 
                                new DataMonitor(treeManager, valNode.getFullPath(), segmentSize, severityNode.getFullPath(), ignFuture));
                            ctxt.pendIO(5.);
                        }
                        else //Periodic
                        {
                            TreeNode periodNode = tree.getNode(nodeName+":PERIOD");
                            float period = periodNode.getFloat();
                            long periodMs = (long)(period*1000);
                            if(period > getThreshold)
                                (new Thread(new DataScanner(treeManager, valNode.getFullPath(),  segmentSize, severityNode.getFullPath(), valChan, ctxt, periodMs, 
                                        ignFuture))).start();
                        }
                    }catch(Exception exc)
                    {
                        System.err.println("Error handling record "+ recName + ": " + exc);
                    }
                }
            }
        }catch(Exception exc)
        {
            System.err.println("Generic error: "+ exc);
	    System.exit(0);
        }

	if(debug) System.out.println("All PV Added");


	BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
	while(true)
	{
	    System.out.println("MDSplus pulse file "+ experiment + " shot " + shot);
	    System.out.println("(q) quit (i) PV list");
	    try {
	    	java.lang.String cmd = br.readLine();
	    	if(cmd.equals("q"))
		    System.exit(0);
	    	if(cmd.equals("i"))
	    	{
		    int index = 0;
		    Enumeration vars = monitorInfo.keys();
		    while(vars.hasMoreElements())
		    {
			index++;		    
			java.lang.String var = (java.lang.String)vars.nextElement();
		    	int count = ((Integer)monitorInfo.get(var)).intValue();
			int buffSize = getBufSize(tree, tree.getNode(var));
			java.lang.String pvName = getPVname(tree, tree.getNode(var));
		    	System.out.println("["+ index +"]--" + var + "\tPV " + pvName + "\t Mon. count " + count +" Buff. size " + buffSize);
		    }
		}
	    }catch(Exception exc){}	
	}
    }


///////////////ONLY FOR TEST

    static void testMdsAccess()
    {
	TreeManager treeManager = null;
	try {
	    treeManager = new TreeManagerConnection("spilds.rfx.local:8001", "prova", 1);
	}catch(Exception exc)
	{
	    System.out.println("Error creating TreeManagerConnection: " + exc);
	    System.exit(0);
	}
	for(int idx = 0; idx < 200; idx++)
	{
	    for(int i = 0; i < 999; i++)
	    {
		treeManager.putRow("NODO"+(i+1), new Float32(idx), (long)idx, 20);
	    }
	    if(idx%20 == 0)
	    {
	        try {
	        Thread.currentThread().sleep(1000);
	        }catch(InterruptedException exc){}
	    }
	}
	System.out.println("FINITO");
    }

}
