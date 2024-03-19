import java.util.*;
import java.util.concurrent.*;
import gov.aps.jca.*;
import gov.aps.jca.dbr.*;
import gov.aps.jca.event.*;
import java.io.*;
import MDSplus.*;
import java.net.*;
import java.text.*;

/*
javac -cp ./caj-1.1.14.jar:./jca-2.3.6.jar:/usr/local/mdsplus/java/classes/mdsobjects.jar ChannelArchiver.java
cp caj-1.1.14.jar jca-2.3.6.jar /usr/local/mdsplus/epics/archiver/.
cp *.class /usr/local/mdsplus/epics/archiver/.
*/


public class ChannelArchiver
{
    static int SEGMENT_SIZE;
    static int MAX_QUEUE_LEN  = 10000;
    static boolean debug = false;
    static boolean disableAccesPVaux = false;
    static long POSIX_TIME_AT_EPICS_EPOCH = 631152000000000000L;//ns from 00:00 1 January 1990 EPICS epoch

    static Hashtable monitorInfo = new Hashtable();
    static Hashtable scanInfo = new Hashtable();
    static Hashtable streamInfo = new Hashtable();
    static Hashtable bufferH = new Hashtable(); //Indexed by path name
    static Vector<DataMonitor> dataMonitors = new Vector<DataMonitor>();
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

	if(dbr.isENUM())
	    System.out.print("IS ENUM TYPE");

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
	                   System.err.println("Unexpected data type for node "+ getNodeName());
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
                        if( tree != null )
                        {
                            currSize = tree.getDatafileSize();
                            if(debug) System.out.println("QUEUE SIZE: " + queue.size());
                            if(debug) System.out.println("FILE SIZE: " + currSize);
                            if(currSize > 1000000000 && currSize == prevSize)
                            ChannelArchiver.debug = true;
                            prevSize = currSize;
                        }
                    } catch(Exception exc){System.err.println("Cannot get Datafile Size: " + exc);
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
	boolean terminate = false;

	int currShot;
	int newShot;
	Tree model;
	Tree tree;
        boolean isTrendShot = false;
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

	    if( shot > 0 )
	    {
                System.out.println("Experiment "+expName);
                tree = new Tree(expName, shot);
                isTrendShot = false;
	    }
	    else
            {
                tree = null;
                isTrendShot = true;
	    }
	    currShot = newShot = shot;
            sizeChecker = new SizeChecker(tree);
            (new Thread(sizeChecker)).start();
	}

        public void setNewShot(int shot)
        {
            if( shot < 0 )
            {
                System.out.println("setNewShot terminate");
                terminate = true;
	    }
            else
            {
                newShot = shot;
                currShot = -1;
                terminate = false;
	    }
        }

        public int getCurrShot()
        {
            return currShot;
        }

	public void terminate()
        {
            terminate = true;
	}

		public void run()
		{

		    DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd");
		    Calendar calendarData = Calendar.getInstance();
		    Date currDate;
		    int  currDateInt; 

			System.err.println("Start TreeHandler on " + newShot );

			while(true)
			//while(!terminate)
			{

			    if ( terminate && queue.isEmpty() )
			    {
			         System.err.println("Stop TreeHandler on " + newShot );
			         break;
			    }

		        try {
		            currDate    = new Date();
		            currDateInt = Integer.parseInt(dateFormat.format(currDate)); 

				    //if ( !isTrendShot && currShot == currDateInt )
				    if ( !isTrendShot  )
				    {
				        calendarData.setTime(currDate);
				        calendarData.add(Calendar.DATE, -1);
				        int yesterdayDateInt = Integer.parseInt(dateFormat.format(calendarData.getTime()));
				        
					    if ( currShot ==  yesterdayDateInt ) //Date is changed
						{  
						     System.err.println("Change date " );
						     System.err.println("Curr date new  shot " + currDateInt );
						     System.err.println("Curr shot curr shot " + currShot );
				             newShot = currDateInt;
						}
					}
		        } catch(Exception exc){System.err.println("Error, shot number on date pulse calculation : " + exc);}

				if( newShot > 0 && ( newShot != currShot ) || ( currSize > switchSize ) )
				{
					if(currSize > switchSize)
					{
		 			    if(debug) System.out.println("REACHED FILE SIZE LIMIT: " + currSize + " " + switchSize);
			   		    currShot++;
					    newShot = currShot;
					}
				    else
				    {
					    currShot = newShot;
				    }

					try {
			            if( !isTrendShot )
			            {
			 		        System.out.println("CREATE EXP "+ expName +" NEW SHOT: " + newShot );
					        model.createPulse(currShot);
			            }
			            else
			 		        System.out.println("OPEN EXP "+ expName +" NEW SHOT: " + newShot );

					    try
			            {
					        tree = new Tree(expName, currShot);
			            } 
		                catch(Exception exc)
		                {
			                if( isTrendShot )
						    {
						        System.out.println("WARNING: Trend shot pulse file  "+ expName +" " + newShot + " does not exist" );
			 		            System.out.println("WARNING: CREATE EXP "+ expName +"   SHOT: " + newShot );
					            model.createPulse(currShot);
			                    tree = new Tree(expName, currShot);
					        } else
			                    throw(exc);
					    }
					    treeNodeHash.clear();
					    sizeChecker.terminate();
					    currSize = 0;
					    sizeChecker = new SizeChecker(tree);
					    (new Thread(sizeChecker)).start();
					} catch(Exception exc){System.err.println("Error creating pulse: " + exc);}
				}


				TreeDataDescriptor descr = null;
			    try {
			        descr = queue.take();
				} 
                catch(Exception exc)
                {
                    System.err.println("Warning dequeuing request: "+exc); 
                    //System.exit(0);
                    break; //2021 5 21
                }
				java.lang.String nodeName = descr.getNodeName();
			    TreeNode node = (TreeNode)treeNodeHash.get(nodeName);
				if(node == null)
				{
				    try {
				        node = tree.getNode(nodeName);
				        treeNodeHash.put(nodeName, node);
				    }
				    catch(Exception exc)
				    {
					    System.err.println("Error getting tree node for " + nodeName + ": " +exc + " : " + tree);
				 	    System.exit(0);
				    }
				}
				try 
		        {
			        //System.out.println(" Write data on Node " + nodeName );
				    if(descr.getDim() > 1)
				    {
				        node.makeTimestampedSegment(descr.getVals(), descr.getTimes());
				    }
			        else
				    {
				        node.putRow(descr.getVal(), descr.getTimes()[0]);
				    }
				} catch(Exception exc){ System.err.println("Error in TimestampedSegment: " + exc);}
			}
			treeNodeHash.clear();
			sizeChecker.terminate();
			currSize = 0;
			tree = null;
			System.err.println("Terminate TreeHandler on " + currShot );
		}
    } //End static inner  class TreeHandler


    //TreeHandlerConnection Manages insertion of data into MDSplus tree using thin client and GetMany
    static class TreeHandlerConnection implements Runnable
    {
	    Connection connection;
	boolean terminate = false;
	java.lang.String expName;
	LinkedBlockingQueue<TreeDataDescriptor> queue;
	TreeHandlerConnection(java.lang.String ipAddress, java.lang.String expName, int shot,LinkedBlockingQueue<TreeDataDescriptor> queue) throws Exception
	{
	    this.queue = queue;
		connection = new Connection(ipAddress);
		connection.openTree(expName, shot);
	}

	public void terminate()
	{
	      terminate = true;
	}


	public void run()
	{
		long startTime, endTime;
	    //while(true)
	    while(!terminate)
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
				    getMany.append("put_"+count, "PutRow("+descr.getNodeName()+",1000,"+descr.getTimes()[0]+"Q,"+descr.getVal().toString()+")", new Data[0]);
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
				    if(debug) System.out.println("Writing "+count+ " data items");
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

	ShotPulseUpdater shotPulseUpdater;

	class ShotPulseDuration implements Runnable
	{
	    int pulseDuration = 0;

	    ShotPulseDuration(int pulseDuration)
	    {
	        this.pulseDuration = pulseDuration;
	    }

	    public void run()
	    {
		    try {
			     System.out.println("Acquisition duration  : " + pulseDuration);
			     Thread.currentThread().sleep( pulseDuration * 1000 );
			     TreeManager.this.setNewShot(-1);
			     System.out.println("Shot duration expired : " + pulseDuration);
		    }catch(Exception exc){
			    System.err.println("ShotPulseDuration fatal exception : " + exc);
		    }
	    }

	}//End static inner class ShotPulseDuration


	class ShotPulseUpdater implements Runnable
	{
	    boolean terminated = false;
	    int shot;
	    int port;

	    ShotPulseUpdater(int port)
	    {
	        this.port = port;
			shot = -1;
	    }

	    void terminate()
	    {
	        terminated = true;
	    }
	    public void run()
	    {
		    try {
		        ServerSocket shotServerSocket = new ServerSocket(port);
			    while(!terminated)
			    {
				    try {
			       		System.out.println("Waiting for command....");
					    Socket connectionSocket = shotServerSocket.accept();
		       			BufferedReader inFromClient = new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
	                    StringTokenizer st = new StringTokenizer(inFromClient.readLine());
	                    java.lang.String command = st.nextToken();
			       		System.out.println("Received COMMAND     : " + command);
					    if ( command.equals("start"))
					    {
			       			shot = Integer.parseInt( st.nextToken() );
			                int duration = Integer.parseInt( st.nextToken() );
			       			System.out.println("Received shot     : " + shot);
			       			System.out.println("Received duration : " + duration);
	                        System.out.println("Start Acquisition");
						    TreeManager.this.setNewShot(shot);
			                TreeManager.this.startPulseDuration(duration);
					    }
					    else
					    {
						    if( command.equals("stop") )
						    {
							    TreeManager.this.setNewShot(-1);
						    }
					    }
					    connectionSocket.close();

				    }catch(NumberFormatException exc){
				        System.err.println("Cannot get shot number : " + exc);
				    }
		        }
		    }catch(Exception exc){
			    System.err.println("ShotPulseUpdater fatal exception : " + exc);
		    }
	    }
	}//End static inner class shotPulseUpdater


	TreeHandler treeH;
	Thread threadTreeH = null;
	Hashtable nodeHash = new Hashtable();
	LinkedBlockingQueue<TreeDataDescriptor> queue;
	    Tree model;
	TreeManager(java.lang.String expName, Tree model, int shot, long switchSize) throws Exception
	{
	    queue = new LinkedBlockingQueue<TreeDataDescriptor>(MAX_QUEUE_LEN);
	    treeH = new TreeHandler(expName, model, shot, switchSize, queue);
		this.model = model;
	    if( shot > 0 )
	    {
	        threadTreeH = new Thread(treeH);
	        threadTreeH.start();
	    }
	}

	    TreeManager(){}


	    public void startPulseDuration(int duration)
	    {
		if( duration > 0 )
		    (new Thread(new ShotPulseDuration(duration))).start();
	}

	    public void startShotUpdater(int port)
	    {
		shotPulseUpdater = new ShotPulseUpdater(port);
		(new Thread(shotPulseUpdater)).start();
	    }

	    public void pvsSnap()
	    {
		for(int i = 0; i < dataMonitors.size(); i++)
		    dataMonitors.elementAt(i).refresh();
	    }


	public void setNewShot(int shot)
	{

	    if ( threadTreeH != null && threadTreeH.isAlive() && getCurrShot() == shot )
	        return;

		if( shot == -1 || getCurrShot() > 0 )
		{
	         System.out.println("SNAP last PV values in shot " + getCurrShot());
	         pvsSnap();
	    }

	    if ( threadTreeH != null && threadTreeH.isAlive() && getCurrShot() != shot )
		{

		    treeH.setNewShot(-1);
	        try {
		        System.out.println("threadTreeH Thread state " + threadTreeH.getState() );

                if( threadTreeH.getState().equals(Thread.State.WAITING) ) 
                {
   			        System.err.println("threadTreeH interrupt");
                    threadTreeH.interrupt();
		        }
			    System.err.println("threadTreeH Stop Thread " + threadTreeH.getState() );

		        System.err.println("threadTreeH Stop Thread");
		        //threadTreeH.join();
		        threadTreeH.interrupt();
		        System.out.println("threadTreeH Thread state " + threadTreeH.getState() );
	        }
	        catch(Exception exc)
	        {
	            System.err.println("threadTreeH " + exc);
	        }

	        threadTreeH = null;
	        nodeHash.clear();
	    }

		treeH.setNewShot(shot);
	    if( shot > 0  )
	    {
	        System.err.println("setNewShot Start Thread");
		    threadTreeH = new Thread(treeH);
	        threadTreeH.start();
	        System.out.println("SNAP first PV values");
	        pvsSnap();
	    }
	    else
	    {
	        threadTreeH = null;
	        nodeHash.clear();
	    }
	}

	public int getCurrShot()
	{
		return treeH.getCurrShot();
	}

	synchronized void streamRow(java.lang.String treeNodeName, Data data, long time)
	{


	     java.lang.String recName = (java.lang.String)streamInfo.get(treeNodeName);
	     if(recName == null)
	        return;

//System.out.println("Spedisco evento  " + recName);

	     long timeUTC = (time + POSIX_TIME_AT_EPICS_EPOCH)/1000000;


	     try {
	        java.lang.String streamEvent = ""+getCurrShot()+" "+recName+" L 1 "+timeUTC+" "+data.getFloat();

//System.out.println("Spedisco dato  " + timeUTC);


	        Event.setEventRaw("STREAMING", streamEvent.getBytes());
	        }catch(Exception exc)
	        {
	            System.out.println("Cannot send Stream Event for " + recName);
	        }
	}
	synchronized void putRow(java.lang.String treeNodeName, Data data, long time, int segmentSize)
	{
	    if( threadTreeH == null || !threadTreeH.isAlive() )
	       return;

		TreeDataDescriptor descr = (TreeDataDescriptor)nodeHash.get(treeNodeName);
	    if(descr == null)
	    {
	        try {
	            nodeHash.put(treeNodeName, descr = new TreeDataDescriptor(treeNodeName, segmentSize));
	        }
	        catch(Exception exc)
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
	            	System.out.println("WARNING: discarded block for " + descr.getNodeName() +" ( Capacity = " + queue.remainingCapacity() + " )");
		    } 

	    }
	    catch(Exception exc){System.err.println("Error enqueuing putRow request");}
	}

	    Tree getTreePulse()
	    {
	       return treeH.tree;
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
	        }
	        catch(Exception exc)
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
	            	//System.out.println("WARNING: discarded block for " + descr.getNodeName());
	            	System.out.println("WARNING: discarded block for " + descr.getNodeName() +" ( Capacity = " + queue.remainingCapacity() + " )");

		    } 

	    }
	    catch(Exception exc){System.err.println("Error enqueuing putRow request");}
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
        long prevStreamTime = 0;
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
	    try
	    {
	        Data data = DBR2Data(dbr);
	        long time = DBR2Time(dbr);
			int severity = CAStatus2Severity(e.getStatus());
	        if(time <= prevTime)  //A previous sample has been received
	        {
                if(time == 0)
				{
	               System.out.println("PV time stamp is 0 it is set to serrver current time: "+time);
 			       time = (System.currentTimeMillis()*1000000L) - POSIX_TIME_AT_EPICS_EPOCH;
                } else { 
	               System.out.println("PREVIOUS SAMPLE!!! Time: "+time + " Previous time: " + prevTime);
	               return;
	            }
	        }
	        if(prevTime > 0 && ((time - prevTime)/1E12 > ignFuture)) //Too far in future
			{
	            System.out.println("Too far in future ignFuture " +ignFuture + "(time - prevTime)" + (time - prevTime) );
	            return;
			}
	        prevTime = time;
	        if(saveTree)
	        {
                    long currStreamTime = java.lang.System.currentTimeMillis();
		    if(currStreamTime > prevStreamTime + 100) //No more often than 10 Hz
		    {
	                treeManager.streamRow(treeNodeName, data, time);
		        prevStreamTime = currStreamTime;
                    }
	            treeManager.putRow(treeNodeName, data, time, segmentSize);
	        }
			if(severity != prevSeverity)
			{
			     treeManager.putRow(severityNodeName, new Int8((byte)severity), time, segmentSize);
		    	 prevSeverity = severity;
	        }
	        currData = data;
	        currTime = time;
			currSeverity = severity;
	    }
	    catch(Exception exc)
	    {
	        exc.printStackTrace();
	        System.err.println("Error writing sample: " + exc);
	    }
	}
	public synchronized DataAndTime getDataAndTime()
	{
	    return new DataAndTime(currData, currTime);
	}

	    public synchronized void refresh()
	    {
		try {
			Date now = new Date();
			long nowTime1 = (now.getTime() * 1000000L) - POSIX_TIME_AT_EPICS_EPOCH;
			long nowTime = (System.currentTimeMillis()*1000000L) - POSIX_TIME_AT_EPICS_EPOCH;


	                //System.err.println("Writing SNAP VALUE " + treeNodeName + " " + currTime + " " + nowTime + " " + currData);
	         treeManager.putRow(treeNodeName, currData, nowTime, segmentSize);
	    }
	    catch(Exception exc)
	    {
	        exc.printStackTrace();
	        System.err.println("Error writing sample: " + exc);
	    }
	    }

	    public synchronized int getSeverity() { return currSeverity;}

    }//End static class

    static Integer _lock_ = new Integer(1);


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
	        long period, int ignFuture)
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
		prevSeverity = -1;
	}

	public void run()
	{
	    while(true)
	    {
	        long time;
	        Data data;
	        Data prevData = null;
			int severity = -1;

	        try {
	            Thread.currentThread().sleep(period);
	        } catch(InterruptedException exc){return;}
	        try {
	            if(mode == SCAN)
	            {
			        synchronized( _lock_ )
	                {
	                    if(dataType == null)
	                    {
	                        if(debug) System.out.println("Starting thread for " + treeNodeName);
	                        prevTime = (System.currentTimeMillis() - 631152000000l ) * 1000000l;
	                        System.out.println("Time at thread start " + prevTime);
	                        DBR dbr = chan.get();
				            try
				            {
	                            dataType = DBR2TimedType(dbr);
	                        } catch (Exception exc ) {
	                            System.out.println("NODE - "+treeNodeName+" Not Scanned invalid time time data format");
	                                        //System.out.println(exc);
					            break;
				            }
	                        nItems = DBR2NItems(dbr);
	                    }

	                    try
	                    {
				            DBR dbr = chan.get(dataType, nItems);
	                       ctxt.pendIO(.5);
	                       data = DBR2Data(dbr);
	                       time = DBR2Time(dbr);
	                       prevData = data;
	                    }
	                    catch (gov.aps.jca.TimeoutException exc)
	                    {
	                       if( prevData == null)
				            {
	                            System.out.print(" prevData NULL ");
					            throw exc;
	                        }
	                        else
	                            System.out.print(" TIMEOUT on data  " + treeNodeName + "Used previous value");
				            data = prevData;
	                        //time = prevTime + period * 1000000;//epics time in nano seconds
	                        time = (System.currentTimeMillis() - 631152000000l ) * 1000000l;
	                    }
	                    catch(Exception exc)
	                    {
	                        System.err.println("DBR get : " + exc);
	                        continue;
	                    }
	                }
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
			    {
				    time = prevTime + period * 1000000;//epics time in nano seconds
	                //System.out.println("---PREVIOUS SAMPLE!!! Time: "+time + " Previous time: " + prevTime);
	                //continue;
	            }

	            //if(prevTime > 0 && ((time - prevTime)/1E9 > ignFuture)) //Too far in future
	            if(prevTime > 0 && ((time - prevTime)/1E12 > ignFuture)) //Too far in future
			    {
	                System.out.println("Too far in future ignFuture " + ignFuture + "(time - prevTime)" + (time - prevTime) );
	                continue;
			    }
	            prevTime = time;

	            try
	            {
	                treeManager.streamRow(treeNodeName, data, time);
	                treeManager.putRow(treeNodeName, data, time, segmentSize);
	            }
	            catch(Exception exc)
	            {
	                System.err.println("treeManager " + treeNodeName+" : " + exc);
		      //System.out.println("NODE " + treeNodeName + " segmentSize " + segmentSize);
	            }

			    if(severity != prevSeverity)
			    {
			       treeManager.putRow(severityNodeName, new Int8((byte)severity), time, segmentSize);
			       prevSeverity = severity;
			    }
	        }catch(Exception exc)
	        {
	            System.err.println("Error writing sample "+treeNodeName+" : " + exc);
	    //System.out.println("NODE " + treeNodeName + " segmentSize " + segmentSize);
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
	    }
        catch(Exception exc)
	    {
		    System.out.println("INTERNAL ERROR in getPVname(): " + exc);
	    }
	    return "";
    }

    static java.lang.String  getPVstrField(MDSplus.Tree tree, MDSplus.TreeNode node, java.lang.String pvField)
    {
	    try {
		    int nid = node.getNid();
		    MDSplus.TreeNode currNode = node;
		    while(nid != 0)
		    {
			    currNode = currNode.getParent();
			    nid = currNode.getNid();
			    try {
			       MDSplus.TreeNode bufSizeNode = tree.getNode(currNode.getFullPath()+":"+pvField);
			       java.lang.String  strFieldValue = bufSizeNode.getData().getString();
			       return strFieldValue;
			    } catch(Exception exc){}
		    }
	    }
        catch(Exception exc)
	    {
		    System.out.println("INTERNAL ERROR in java.lang.String(): " + exc);
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

    static void refresh()
    {
	    for(int i = 0; i < dataMonitors.size(); i++)
		dataMonitors.elementAt(i).refresh();
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
	    TreeManager treeManager = null;
	int ipPort = 0;


	System.out.println("Channel Archiver START");

	    java.lang.String mdsipAddress = System.getProperty("mdsip");
	    java.lang.String debugTxt = System.getProperty("debug");
	    debug = (debugTxt != null && debugTxt.equals("yes"));
	    java.lang.String createPulseTxt = System.getProperty("create-pulse");
	    createPulse = (createPulseTxt != null && createPulseTxt.equals("yes"));

	    java.lang.String disableAccesPVauxTxt = System.getProperty("disable-pv-aux");
	    disableAccesPVaux = (disableAccesPVauxTxt != null && disableAccesPVauxTxt.equals("yes"));


	//debug = true;

//testMdsAccess();
//if(true) return;

	if(args.length != 3 && args.length != 4)
	{
	    System.out.println("Usage:java ChannelArchiver <TREND|SHOT> <experiment> <shot> [segment_size]");
	    return;
	}
	System.out.println("MDSplus ChannelArchiver started, waiting for colecting process variables....");
	java.lang.String archiverMode = args[0];
	java.lang.String experiment = args[1];
	int shot = Integer.parseInt(args[2]);
	if(args.length == 4)
		SEGMENT_SIZE = Integer.parseInt(args[3]);
	else
		SEGMENT_SIZE = 1;

	try {
	    JCALibrary jca = JCALibrary.getInstance();
	    Context ctxt = jca.createContext(JCALibrary.CHANNEL_ACCESS_JAVA ) ;

	    System.out.println("gov.aps.jca.Context.auto_addr_list " + jca.getProperty("gov.aps.jca.Context.auto_addr_list"));
	    System.out.println("gov.aps.jca.Context.addr_list " + jca.getProperty("gov.aps.jca.Context.addr_list"));

            System.out.println("Experiment "+experiment);

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


		if(mdsipAddress == null)
			treeManager = new TreeManager(experiment, tree, shot, fileSize);
		else
			treeManager = new TreeManagerConnection(mdsipAddress, experiment, shot);
	    TreeNodeArray treeNodeArr = tree.getNodeWild("***");
	    java.lang.String []nodeNames = treeNodeArr.getFullPath();
	    int[] nids = treeNodeArr.getNid();

	    for(int i = 0; i < nodeNames.length; i++)
	    {
	        if(nodeNames[i].endsWith(":REC_NAME"))
	        {
	            java.lang.String nodeName = nodeNames[i].substring(0, nodeNames[i].length() - 9);
	            java.lang.String recName  = "";

                if ( (!new TreeNode(nids[i], tree).isOn()) ){
					System.out.println("NODE "+nodeName+" OFF");
                    continue;
				}


	            try {
	                recName = new TreeNode(nids[i], tree).getData().getString();
				    recName = recName.trim();
	                if(debug) System.out.println("---Scanning...."+recName);
	                //Get VAL channel. It will remain open thorough the whole program execution
	                Channel valChan = ctxt.createChannel(recName+".VAL");
	                ctxt.pendIO(5.);
	                DBR valDbr = valChan.get();
	                ctxt.pendIO(5.);
				    if(debug) System.out.println("Channel created.");
	                if(!valDbr.isENUM() && !valDbr.isCTRL() && !valDbr.isINT() && !disableAccesPVaux )
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
	                         System.err.println("Cannot get EGU for " + recName + " Node :"+ nodeName +":EGU : " + exc);
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
	                         System.err.println("Cannot get HOPR for " + recName+ " Node :"+ nodeName +":HOPR : " + exc);
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
	                        System.err.println("Cannot get LOPR for " + recName + " Node :"+ nodeName +":LOPR : " + exc);
	                    }
	                }


	                //Get SCAN mode for this channel
	                TreeNode valNode = tree.getNode(nodeName+":VAL");
	                TreeNode scanNode = tree.getNode(nodeName+":SCAN_MODE");
	                TreeNode severityNode = tree.getNode(nodeName+":ALARM");
	                java.lang.String scanMode = scanNode.getString().toUpperCase();
	                if(scanMode.equals("MONITOR+STREAMING") || scanMode.equals("PERIODIC+STREAMING"))
	                    streamInfo.put(valNode.getFullPath(), recName);
				    int segmentSize = getBufSize(tree, valNode);
				    if(debug) System.out.println("Monitoring channel started. Segment size: "+segmentSize);
	                if(scanMode.equals("MONITOR") || scanMode.equals("MONITOR+STREAMING"))
	                {
				        DataMonitor newDataMonitor;
	                    if(valDbr.isENUM() || valDbr.isCTRL() || valDbr.isINT())
	                        valChan.addMonitor(DBRType.TIME_INT, 1, Monitor.VALUE,
	                       	    newDataMonitor = new DataMonitor(treeManager, valNode.getFullPath(), segmentSize, severityNode.getFullPath(), ignFuture));
	                    else
	                        valChan.addMonitor(DBRType.TIME_DOUBLE, 1, Monitor.VALUE,
	                            newDataMonitor = new DataMonitor(treeManager, valNode.getFullPath(), segmentSize, severityNode.getFullPath(), ignFuture));
	                    ctxt.pendIO(5.);
				        dataMonitors.addElement(newDataMonitor);
				        if(debug) System.out.println("Create monitor :"+ valNode.getFullPath());
	                }
	                else //Periodic
	                {
	                    TreeNode periodNode = tree.getNode(nodeName+":PERIOD");
	                    float period = periodNode.getFloat();
	                    long periodMs = (long)(period*1000);
	                    //if(period > getThreshold)
	                    {
					        if(debug) System.out.println("Create thread :"+ valNode.getFullPath());
	                        Thread t = new Thread( new DataScanner(treeManager, valNode.getFullPath(),  segmentSize, severityNode.getFullPath(),
	                                                     valChan, ctxt, periodMs,ignFuture) );
	                        //t.start();
	                        scanInfo.put(valNode.getFullPath(), t);
	                     }
	                }
	            }
	            catch(Exception exc)
	            {
	                exc.printStackTrace();
	                System.err.println("Error handling record "+ recName + " Node :"+ nodeName + ": " + exc);
	            }
	        }
	    }

	    Enumeration vars = scanInfo.keys();
	    while(vars.hasMoreElements())
		{
			java.lang.String var = (java.lang.String)vars.nextElement();
			((Thread)scanInfo.get(var)).start();
	    }

	}catch(Exception exc)
	{
	    System.err.println("Generic error: "+ exc);
	    System.exit(0);
	}

	    if(debug) System.out.println("All PV Added");


	if(  archiverMode.equals("SHOT") )
	{
	    try {
	     TreeNode node = tree.getNode(":SHOT_PORT");
	     ipPort = node.getInt();
	     //Add check if port is in use
	    }catch(Exception exc)
	    {
	        try {
	            TreeNode node = tree.getNode(".TSHOT_CONF:ARCH_PORT");
	            ipPort = node.getInt();
	           //Add check if port is in use
            }catch(Exception exc1) {
	           ipPort = 9999;
            }
	    }
	    treeManager.startShotUpdater(ipPort);
	    System.out.println("NEW Trend shot server for experiment " + experiment + " communication port " + ipPort);
	}

	    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
	    while(true)
	    {
		if(  archiverMode.equals("TREND") )
	    {
		    System.out.println("MDSplus TREND pulse file "+ experiment + " shot " + treeManager.getCurrShot());
			System.out.println("(q) quit (i) PV list");
	    }
	    else
		{
		    System.out.println("(q) quit (i) PV list (s) Create new pulse file");
	    }
		try {
			java.lang.String cmd = br.readLine();
            if ( cmd == null )//stated in back ground
            {
                System.out.println("Trend acquisition started in back ground");
                java.lang.Thread.currentThread().join();
            }

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
                    java.lang.String scanMode = getPVstrField(tree, tree.getNode(var), "SCAN_MODE");
	                int dataSize = 0;
			        if(treeManager.getTreePulse() != null) 
                                {
                                    Tree treeShot = new Tree(treeManager.getTreePulse().getName(), treeManager.getTreePulse().getShot() );
		                    //dataSize = (treeManager.getTreePulse().getNode(var)).getLength();
                                    dataSize = treeShot.getNode(var).getLength();
                                    treeShot.close();
                                }

		            System.out.format("[%5d]-%-18s-%-45s%-25s Mon. count %8d Data Stored %8d Buff. size %3d\n", index, scanMode, var, pvName, count, dataSize, buffSize );
				   //System.out.println("["+ index +"]-MONITOR-" + var + "\tPV " + pvName + "\t Mon. count " + count +" Buff. size " + buffSize);
				}

				index = 0;
			    vars = scanInfo.keys();
				while(vars.hasMoreElements())
				{
					index++;
					java.lang.String var = (java.lang.String)vars.nextElement();
					Thread t = ((Thread)scanInfo.get(var));
					int buffSize = getBufSize(tree, tree.getNode(var));
					java.lang.String pvName = getPVname(tree, tree.getNode(var));
                    java.lang.String scanMode = getPVstrField(tree, tree.getNode(var), "SCAN_MODE");
	                int dataSize = 0;
				    if(treeManager.getTreePulse() != null) 
			            dataSize = (treeManager.getTreePulse().getNode(var)).getLength();
                     
			        System.out.format("[%5d]-%-18s-%-45s%-25s Thread is alive %5s Data Stored %8d Buff. size %3d\n", index, scanMode,  var, pvName,  (t.isAlive() ?"true":"false"), dataSize, buffSize  );
					//System.out.println("["+ index +"]-SCAN-" + var + "\tPV " + pvName + "\t Tread is alive " + t.isAlive() +" Data store "+ dataSize +" Buff. size " + buffSize);
				}
	            System.out.println("Trend shot server for experiment " + experiment + " communication port " + ipPort);
	 		 }

	         if(  archiverMode.equals("SHOT") )
			 {
				 if(cmd.equals("s"))
				 {
				    System.out.print("Shot ");
				    shot = Integer.parseInt(br.readLine());
				    treeManager.setNewShot(shot);
				 }
			 }
		} catch(Exception exc){
          exc.printStackTrace();
          System.out.println("Fatal : " + exc);
          break;}
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




