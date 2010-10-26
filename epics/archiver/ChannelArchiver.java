import java.util.*;
import java.util.concurrent.*;
import gov.aps.jca.*;
import gov.aps.jca.dbr.*;
import gov.aps.jca.event.*;

import MDSplus.*;


public class ChannelArchiver 
{
    static int SEGMENT_SIZE = 300;
    static int MAX_QUEUE_LEN  = 10000;
    static boolean debug = false;
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
    
    
    static class GroupHandler
    {
        java.lang.String groupName;
        Vector disableVect = new Vector();
        Data checkExpr;
        boolean disabled;
        GroupHandler(java.lang.String groupName, java.lang.String checkExprStr)
        {            
            this.groupName = groupName;
            try {
                checkExpr = Data.compile(checkExprStr);
                disabled = false;
            }catch(Exception exc)
            {
                System.err.println("Error compiling check expression "+checkExprStr+" for group " + groupName + ": " + exc);
                disabled = false;
            }
        }
        void addCheckChan(java.lang.String chanName)
        {
            disableVect.add(chanName);
        }
        boolean isCheckChan(java.lang.String chanName)
        {
            return disableVect.contains(chanName);
        }
        void updateCheckData(java.lang.String chanName, Data data)
        {
            try {
                Data setVar = Data.compile("_"+chanName+"=$1", new Data[]{data});
                setVar.data();
                int ris = checkExpr.data().getInt();
                if(ris != 0)
                    disabled = true;
                else
                    disabled = false;
             } catch(Exception exc){System.err.println("Error setting TDI variable _" + chanName);}
        }
        boolean isDisabled()
        {
            return disabled;
        }
    }
            
    static class TreeDataDescriptor
    {
    	java.lang.String nodeName;
   	double []vals;
    	long []times;
    	int idx;
    	TreeDataDescriptor(java.lang.String nodeName)
    	{
	    vals = new double[SEGMENT_SIZE];
	    times = new long[SEGMENT_SIZE];
	    idx = 0;
	    this.nodeName = nodeName;
    	}
	java.lang.String getNodeName(){return nodeName;}
    	boolean addRow(double val, long time)
    	{
	    if(idx < SEGMENT_SIZE)
	    {
	    	vals[idx] = val;
	    	times[idx] = time;
	    	idx++;
	    }
            return idx == SEGMENT_SIZE;
    	}
	double [] getVals() {return vals;}
	long [] getTimes() { return times;}
    } //End static inner class TreeDataDescriptor
    //Manages insertion of data into MDSplus tree
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
                while(!terminated)
                {
                    try {
			System.out.println("QUEUE SIZE: " + queue.size());
                        currSize = tree.getDatafileSize();
			System.out.println("FILE SIZE: " + currSize);
			if(currSize > 1000000000 && currSize == prevSize)
			    ChannelArchiver.debug = true;
			prevSize = currSize;

                    }catch(Exception exc){System.err.println("Cannot get Datafile Size: " + exc); /*System.exit(0);*/}
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
	   	    node.makeTimestampedSegment(new Float64Array(descr.getVals()), descr.getTimes());
	   	}catch(Exception exc){ System.err.println("Error in putTimestampedSegment: " + exc);}
            	if(currSize > switchSize)
            	{
 		    System.out.println("REACHED FILE SIZE LIMIT: " + currSize + " " + switchSize);
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
        synchronized void putRow(java.lang.String treeNodeName, Data data, long time)
        {
	    TreeDataDescriptor descr = (TreeDataDescriptor)nodeHash.get(treeNodeName);
            if(descr == null)
            {
                try {
                    nodeHash.put(treeNodeName, descr = new TreeDataDescriptor(treeNodeName));
                }catch(Exception exc)
                {
                    System.err.println("INTERNAL ERROR: Cannot get node for "+treeNodeName + ": " + exc);
                    return;
                }
            }
            try {
              	if(descr.addRow(data.getDouble(), time))
	    	{
                    nodeHash.put(descr.getNodeName(), new TreeDataDescriptor(descr.getNodeName()));
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
        java.lang.String chanName;
        boolean isCheckChan;
        GroupHandler gh;
        boolean saveTree;
        Data currData = null;
        long currTime;
        long prevTime = 0;
        int ignFuture;
        public DataMonitor(TreeManager treeManager, java.lang.String treeNodeName, int ignFuture, 
              java.lang.String chanName, boolean isCheckChan, GroupHandler gh)
        {
            this.treeManager = treeManager;
            this.treeNodeName = treeNodeName;
            saveTree = true;
            this.ignFuture = ignFuture;
            this.chanName = chanName;
            this.isCheckChan = isCheckChan;
            this.gh = gh;
        }
        public DataMonitor(int ignFuture)
        {
            treeManager = null;
            treeNodeName = null;
            saveTree = false;
            this.ignFuture = ignFuture;
        }
 	public synchronized void monitorChanged(MonitorEvent e)
	{
            DBR dbr = e.getDBR();
            try {
                Data data = DBR2Data(dbr);
  
                long time = DBR2Time(dbr);
                if(isCheckChan)
                    gh.updateCheckData(chanName, data);
                if(time <= prevTime)  //A previous sample has been received
                {
                    System.out.println("PREVIOUS SAMPLE!!! Time: "+time + " Previous time: " + prevTime);
                    return;
                }
                if(prevTime > 0 && ((time - prevTime)/1E9 > ignFuture)) //Too far in future
                    return; 
                prevTime = time;
                 if(saveTree)
                 {
                    if (isCheckChan || !gh.isDisabled()) 
                    {
                        treeManager.putRow(treeNodeName, data, time);
                    }
                }
                else
                {
                    currData = data;
                    currTime = time;
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
        
    }//End static class 
    
    static class DataScanner implements Runnable
    {
        static final int SCAN = 1, MONITOR = 2;
        int mode;
        java.lang.String treeNodeName;
        TreeManager treeManager;
        Channel chan;
        Context ctxt;
        long period;
        long prevTime = 0;
        int ignFuture;
        DataMonitor monitor;
        java.lang.String chanName;
        boolean isCheckChan;
        GroupHandler gh;
        DBRType dataType = null;
        int nItems;
        
        public DataScanner(TreeManager treeManager, java.lang.String treeNodeName, Channel chan, Context ctxt, 
                long period, int ignFuture, java.lang.String chanName, boolean isCheckChan, GroupHandler gh)
        {
            this.treeManager = treeManager;
            this.treeNodeName = treeNodeName;
            this.chan = chan;
            this.ctxt = ctxt;
            this.period = period;
            this.chanName = chanName;
            this.isCheckChan = isCheckChan;
            this.gh = gh;
            this.ignFuture = ignFuture;
            mode = SCAN;
            
        }
        public DataScanner(TreeManager treeManager, DataMonitor monitor, java.lang.String treeNodeName, Channel chan, 
                Context ctxt, long period, int ignFuture, java.lang.String chanName, boolean isCheckChan, GroupHandler gh)
        {
            this.treeManager = treeManager;
            this.treeNodeName = treeNodeName;
            this.monitor = monitor;
            this.chan = chan;
            this.ctxt = ctxt;
            this.period = period;
            this.chanName = chanName;
            this.isCheckChan = isCheckChan;
            this.gh = gh;
            this.ignFuture = ignFuture;
            mode = MONITOR;
        }
        public void run()
        {
            while(true)
            {
                long time;
                Data data;
                try {
                    Thread.currentThread().sleep(period);
                } catch(InterruptedException exc){return;}
                if (!isCheckChan && gh.isDisabled()) 
                    continue;

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
                    }
                    else //mode == MONITOR
                    {
                        DataAndTime dataTime = monitor.getDataAndTime();
                        data = dataTime.getData();
                        time = dataTime.getTime();
                    }
                    if(isCheckChan)
                        gh.updateCheckData(chanName, data);
                   if(time <= prevTime)  //A previous sample has been received
                        continue;
                    if(prevTime > 0 && ((time - prevTime)/1E9 > ignFuture)) //Too far in future
                        continue; 
                    prevTime = time;
                    
                    if (isCheckChan || !gh.isDisabled()) 
                        treeManager.putRow(treeNodeName, data, time);
                }catch(Exception exc)
                {
                    System.err.println("Error writing sample: " + exc);
                }
            }
        }
    }

    public static void main(java.lang.String[] args)
    {
        //Global Parameters
        float getThreshold; //treshold above which use real scan instead of monitor scan
        int ignFuture; //Number of seconds above which skip recording
        long fileSize; //DImension above which create a new pulse;
        Hashtable groupH = new Hashtable();
        if(args.length != 2 && args.length != 3)
        {
            System.out.println("Usage:java ChannelArchiver <experiment> <shot> [segment_size]");
            return;
        }
        java.lang.String experiment = args[0];
        int shot = Integer.parseInt(args[1]);
	if(args.length == 3)
	    SEGMENT_SIZE = Integer.parseInt(args[2]);
	System.out.println("SEGMENT SIZE: " + SEGMENT_SIZE);

        try {
            JCALibrary jca = JCALibrary.getInstance();
            Context ctxt = jca.createContext(JCALibrary.CHANNEL_ACCESS_JAVA);
            Tree tree = new Tree(experiment, -1);
//Get Global Parameters            
            try {
             TreeNode node = tree.getNode(":GET_TRESH");
             getThreshold = node.getFloat();
            }catch(Exception exc)
            {
                getThreshold = 20;
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
                fileSize = 1000000;
            }
            

            TreeManager treeManager = new TreeManager(experiment, tree, shot, fileSize);
            TreeNodeArray treeNodeArr = tree.getNodeWild("***");
            java.lang.String []nodeNames = treeNodeArr.getPath();
            int[] nids = treeNodeArr.getNid();
            for(int i = 0; i < nodeNames.length; i++)
            {
                if(nodeNames[i].endsWith(":REC_NAME"))
                {
                    java.lang.String nodeName = nodeNames[i].substring(0, nodeNames[i].length() - 9);
                    java.lang.String recName  = "";
                    boolean isDisable;
                    
                    try {
                        recName = new TreeNode(nids[i], tree).getData().getString();
                        System.out.println(recName);
                        //EGU
                        try {
                            Channel eguChan = ctxt.createChannel(recName+".EGU");
                            ctxt.pendIO(5.);
                            DBR dbr = eguChan.get();
                            ctxt.pendIO(5.);
                            TreeNode eguNode = tree.getNode(nodeName+":EGU");
                            eguNode.putData(DBR2Data(dbr));
                            eguChan.destroy();
                         }catch(Exception exc)
                         {
                             System.err.println("Cannot get EGU for " + recName + ": " + exc);
                             continue;
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
                        //Get VAL channel. It will remain open thorough the whole program execution
                        Channel valChan = ctxt.createChannel(recName+".VAL");
                        ctxt.pendIO(5.);
                        
                        try {
                             TreeNode node = tree.getNode(nodeName+":IS_DISABLE");
                             isDisable = node.getString().toUpperCase().equals("YES");
                        }catch(Exception exc)
                        {
                            isDisable = false;
                        }
                        java.lang.String chanName;
                        GroupHandler gh;
                        try {
                            chanName = tree.getNode(nodeName).findTags()[0];
                            java.lang.String groupName = tree.getNode(nodeName).getParent().getNodeName().toUpperCase().trim();
                            gh = (GroupHandler)groupH.get(groupName);
                            if(gh == null)
                            {
                                TreeNode prevDef = tree.getDefault();
                                tree.setDefault(tree.getNode(nodeName).getParent());
                                java.lang.String checkExpr = tree.getNode(":DISABLE").getData().decompile();
                                tree.setDefault(prevDef);
                                gh = new GroupHandler(groupName, checkExpr);
                                groupH.put(groupName, gh);
                            }
                        }catch(Exception exc)
                        {
                            System.err.println("Error Setting Group Enabling: "+exc);
                            chanName = "";
                            gh = null;
                        }    
                        
                        //Get SCAN mode for this channel
                        TreeNode valNode = tree.getNode(nodeName+":VAL");
                        TreeNode scanNode = tree.getNode(nodeName+":SCAN_MODE");
                        java.lang.String scanMode = scanNode.getString().toUpperCase();
                        if(scanMode.equals("MONITOR"))
                        {
                               valChan.addMonitor(DBRType.TIME_DOUBLE, 1, Monitor.VALUE, 
                                       new DataMonitor(treeManager, valNode.getFullPath(), ignFuture, chanName, isDisable, gh));
                               ctxt.pendIO(5.);
                        }
                        else //Periodic
                        {
                            TreeNode periodNode = tree.getNode(nodeName+":PERIOD");
                            float period = periodNode.getFloat();
                            long periodMs = (long)(period*1000);
                            if(period > getThreshold)
                                (new Thread(new DataScanner(treeManager, valNode.getFullPath(), valChan, ctxt, periodMs, 
                                        ignFuture, chanName, isDisable, gh))).start();
                            else
                            {
                                //Use SCAN-MONITOR
                                DataMonitor monitor = new DataMonitor(ignFuture);
                                valChan.addMonitor(DBRType.TIME_DOUBLE, 1, Monitor.VALUE, monitor);
                                ctxt.pendIO(5.);
                                (new Thread(new DataScanner(treeManager, monitor, valNode.getFullPath(), valChan, ctxt, periodMs, 
                                        ignFuture, chanName, isDisable, gh))).start();

                            }
                        } 
                    }catch(Exception exc)
                    {
                        System.err.println("Error handling record "+ recName + ": " + exc);
                    }
                }
            }
        }catch(Exception exc)
        {
            System.err.println("Geenric error: "+ exc);
        }
    }
}
