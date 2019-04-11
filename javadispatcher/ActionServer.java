import java.util.*;
import java.awt.event.*;
import jScope.*;

class ActionServer implements Server, MdsServerListener, ConnectionListener
{
    Vector<Action> enqueued_actions = new Vector<Action>();
    Hashtable<Integer, Action> doing_actions = new Hashtable<Integer, Action>();
    Vector<ServerListener> server_listeners = new Vector<ServerListener>();
    Hashtable actions       = new Hashtable();
    MdsServer mds_server    = null;
    String tree;
    String server_class;
    String ip_address;
    String subtree;
    int shot;
    boolean ready  = false, //True if the server is ready to participate to the NEXT shot
	    active = false; //True if the server is ready to participate to the CURRENT shot
    Timer timer = new Timer();
    static final int RECONNECT_TIME = 5;
    boolean useJavaServer = true;
    int watchdogPort = -1;

    public ActionServer(String tree, String ip_address, String server_class, boolean useJavaServer, int watchdogPort)
    {
      this (tree, ip_address,  server_class, null, useJavaServer, watchdogPort);
    }
    public ActionServer(String tree, String ip_address, String server_class)
    {
      this (tree, ip_address,  server_class, null, true, -1);
    }

    public ActionServer(String tree, String ip_address, String server_class, String subtree,
	                boolean useJavaServer, int watchdogPort)
    {
	//System.out.println("------------------------------Action Server "+ ip_address);
	this.tree = tree;
	this.server_class = server_class;
	this.ip_address = ip_address;
	this.subtree = subtree;
	this.useJavaServer = useJavaServer;
	this.watchdogPort = watchdogPort;
	try {
	    mds_server = new MdsServer(ip_address, useJavaServer, watchdogPort);
	    mds_server.addMdsServerListener(this);
	    mds_server.addConnectionListener(this);
	    ready = active = true;
	}
	catch(Exception exc)
	{
	  System.out.println("Cannot connect to server " + ip_address + " server class " + server_class);
	  mds_server = null;
	  ready = active = false;
	  startServerPoll();
	}
    }
    public void setTree(String tree) {this.tree = tree; }
    public void setTree(String tree, int shot)
    {
	this.tree = tree;
	this.shot = shot;
    }
   // public boolean isActive() {return mds_server != null; }
    public boolean isActive() {return ready && active; }
    public String getAddress(){return ip_address;}
    public boolean isReady() {if(ready) active = true; return ready;}
    public void processConnectionEvent(ConnectionEvent e)
    {
	if(e.getID() == ConnectionEvent.LOST_CONNECTION)
	{
	    try {
	        if(mds_server != null)
	        {
	            mds_server = null;
	            mds_server.shutdown();
	        }
	        else
	            return; //Already processed
	    }catch(Exception exc)
	    {
	        System.err.println("Error shutting down socket : " + ip_address);
	    }
	    System.out.println("Detected server crash : " + server_class);
	    try {
	        Thread.currentThread().sleep(2000); //Give time to mdsip server to start its own threads
	    }
	    catch (Exception exc) {}

	    processDisconnected(ip_address);

	    java.util.Timer conn_timer = new java.util.Timer();
	    conn_timer.schedule(new TimerTask()
	    {
	        public void run()
	        {
	            //synchronized(ActionServer.this)
	            {
	                try {
	                    mds_server = new MdsServer(ip_address,
	                        useJavaServer, watchdogPort);
	                    mds_server.addMdsServerListener(ActionServer.this);
	                    mds_server.addConnectionListener(ActionServer.this);
	                    mds_server.dispatchCommand("TCL", "SET TREE " + tree + "/SHOT=" + shot);
	                    System.out.println("Restarting server. Class : "+ server_class +" Address: "+ ip_address);
	                    Thread.currentThread().sleep(2000); //Give time to mdsip server to start its own threads
	                }
	                catch (Exception exc) {
	                    mds_server = null;
	                    ready = active = false;
	                    //enqueued_actions.removeAllElements();
	                    startServerPoll();
	                }
	                if (doing_actions.size() > 0) {
	                    Enumeration<Action> doing_list = doing_actions.elements();
	                    while (doing_list.hasMoreElements())
	                        processAbortedNoSynch(doing_list.nextElement()); //in any case aborts action currently being executed
	                    doing_actions.clear();
	                }
	                Action action;
	                if (mds_server == null) {
	                    //synchronized(enqueued_actions)
	                    {
	                        if (enqueued_actions.size() > 0) {
	                            while (enqueued_actions.size() > 0) {
	                                if ( (action = enqueued_actions.elementAt(0)) != null)
	                                    processAbortedNoSynch(action);
	                                enqueued_actions.removeElementAt(0);
	                            }
	                        }
	                    }
	                }
	                else {
	                    ready = active = true;
	                    //synchronized(enqueued_actions)
	                    {
	                        for (int i = 0; i < enqueued_actions.size(); i++) {
	                            action = enqueued_actions.elementAt(i);
	                            try {
	                                //mds_server.dispatchAction(tree, shot, action.getNid(), action.getNid());
	                                action.setServerAddress(ip_address);
	                                System.out.println("-- Action -> " + ip_address);
	                                mds_server.dispatchAction(tree, shot, action.getName(), action.getNid());
	                            }
	                            catch (Exception exc) {}
	                        }
	                    }
	                }
	            }
	        }
	    }, RECONNECT_TIME * 1000);
	}
    }


    public synchronized void addServerListener(ServerListener listener)
    {
	server_listeners.addElement(listener);
    }

    public void pushAction(Action action)
    {

	synchronized (enqueued_actions){
	    action.setServerAddress(ip_address);
	    //System.out.println("-- Action1 -> " + ip_address);
	    enqueued_actions.addElement(action);
	}
	try {
  //        mds_server.dispatchAction(tree, shot, action.getNid(), action.getNid());
	    mds_server.dispatchAction(tree, shot, action.getName(), action.getNid());
	}catch(Exception exc)
	{
	    enqueued_actions.removeElement(action);
	    processAborted(action);
	}
    }



    public /* OCT 2008 synchronized*/ void processMdsServerEvent(MdsServerEvent e)
    {
       int mode = e.getFlags();
       Action doing_action;
	switch(mode) {
	    case MdsServerEvent.SrvJobSTARTING:
	        synchronized(enqueued_actions)
	        {
	            doing_action = enqueued_actions.firstElement();
 //                   doing_action.setServerAddress(this.ip_address);
	            doing_actions.put(new Integer(e.getJobid()), doing_action);
	            enqueued_actions.removeElement(doing_action);
	        }
	        processDoing(doing_action);

	        int timeout = 0;
	        try {
	            Data task = doing_action.getAction().getTask();
	            if(task instanceof RoutineData)
	                timeout = ((RoutineData)task).getTimeout().getInt();
	            else if (task instanceof ProcedureData)
	                timeout = ((ProcedureData)task).getTimeout().getInt();
	            else if (task instanceof MethodData)
	                timeout = ((MethodData)task).getTimeout().getInt();
	            else if (task instanceof ProgramData)
	                timeout = ((ProgramData)task).getTimeout().getInt();
	        }catch(Exception exc){}
	        if(timeout > 0)
	        {
	            timer = new Timer();
	            timer.schedule( new TimerTask() {
	              public void run()
	              {
	                  synchronized (ActionServer.this) {
	                      if (doing_actions.size() != 1)return; //handle only the usual case with 1 doing action
	                      int queue_len = 0;
	                      int size = enqueued_actions.size();
	                      Vector removed_actions = new Vector();
	                      Action curr_action;
	                      try {
	                          mds_server.abort(false); //now we are sure that no other action gets aborted
	                      }
	                      catch (Exception exc) {}

	                  }
	              }
	          }, timeout * 1000);
	        }


	        break;
	    case MdsServerEvent.SrvJobFINISHED :
	        try {
	            timer.cancel();
	        }catch(Exception exc){}

	        //System.out.println("JOBID: " + e.getJobid());

	        if(e.getJobid() == 0) return; //SrvJobFINISHED messages are generated also by SrvCreatePulse and SrvClose
	        Action done_action = doing_actions.remove(new Integer(e.getJobid()));
	        if(done_action == null)
	        {
	            System.out.println("INTERNAL ERROR: received finish message for an action which did not start. Try to restart mdsip server "+ip_address);
	        }
	        else
	        {
	            done_action.setStatus(e.getStatus());
	            processFinished(done_action);
	        }
	        break;
	    case MdsServerEvent.SrvJobABORTED :
	        try {
	            timer.cancel();
	        }catch(Exception exc){}
	        Action aborted_action = doing_actions.remove(new Integer(e.getJobid()));
	        if(aborted_action == null)
	        {
	            System.out.println("INTERNAL ERROR: received abort message for an action which did not start. Try to restart mdsip server "+ip_address);
	        }
	        processAborted(aborted_action);
	        break;
	}
    }

    protected void processFinished(Action action)
    {
	Enumeration<ServerListener> listeners = server_listeners.elements();
	while(listeners.hasMoreElements())
	{
	    ServerListener listener = listeners.nextElement();
	    listener.actionFinished(new ServerEvent(this, action));
	}
    }

    protected void processDoing(Action action)
    {
	action.setServerAddress(this.ip_address);
	Enumeration<ServerListener> listeners = server_listeners.elements();
	while(listeners.hasMoreElements())
	{
	    ServerListener listener = listeners.nextElement();
	    listener.actionStarting(new ServerEvent(this, action));
	}
    }

    protected void processAbortedNoSynch(Action action)
    {

	//System.out.println("-----------------> Aborted action "+ action);

	action.setServerAddress(this.ip_address);

	Enumeration<ServerListener> listeners = server_listeners.elements();
	while(listeners.hasMoreElements())
	{
	    ServerListener listener = listeners.nextElement();
	    listener.actionAborted(new ServerEvent(this, action));
	}
    }

    protected synchronized void processAborted(Action action)
    {
	processAbortedNoSynch(action);
	notify();
    }

    protected void processDisconnected(String msg)
    {
	Enumeration<ServerListener> listeners = server_listeners.elements();
	while(listeners.hasMoreElements())
	{
	    ServerListener listener = listeners.nextElement();
	    listener.disconnected(new ServerEvent(this, msg));
	}
    }

    protected void processConnected(String msg)
    {
	Enumeration<ServerListener> listeners = server_listeners.elements();
	while(listeners.hasMoreElements())
	{
	    ServerListener listener = listeners.nextElement();
	    listener.connected(new ServerEvent(this, msg));
	}
    }

    public Action popAction()
    {
	if(mds_server == null) return  null;
	Descriptor descr;
	try {
	    descr = mds_server.removeLast();
	}catch(Exception exc) {return null; }
	if(descr.getInt() != 0) //there was a pending job in the mds server
	{
	   synchronized(enqueued_actions)
	   {
	        Action removed = enqueued_actions.lastElement();
	        removed.setServerAddress(null);
	        enqueued_actions.removeElement(removed);
	        return removed;
	   }
	}
	return null;
    }

    public Action[] collectActions(String rootPath) {return null;}

    public synchronized Action[] collectActions() {return null; }

    public void beginSequence(int shot)
    {
	this.shot = shot;
	if(mds_server == null || subtree == null || subtree.trim().equals("")) return;
	try {
	    mds_server.createPulse(subtree, shot);
	}catch(Exception exc){}
    }

    public synchronized void endSequence(int shot)
    {
	if(mds_server == null) return;
	try {
	    mds_server.closeTrees();
	}catch(Exception exc) {}
    }

    public synchronized String  getServerClass() {return server_class; }

    public int getQueueLength()
    {
	return enqueued_actions.size();
    }

    public int getDoingAction()
    {
	return this.doing_actions.size();
    }

    public /* OCT 2008 synchronized */ void abort(boolean flush)
    {
	enqueued_actions.removeAllElements();
	if(mds_server == null) return;
	try {
	    mds_server.abort(flush);
	}catch(Exception exc){}
    }

    public /*synchronized*/ boolean abortAction(Action action)
    {
	if((doing_actions.get(new Integer(action.getNid())) == null) && enqueued_actions.indexOf(action) == -1)
	    return false;
	int queue_len = 0;
	synchronized (enqueued_actions) {
	    try {
	        mds_server.abort(false); //now we are sure that no other action gets aborted
	    }
	    catch (Exception exc) {}
	}
	return true;
    }



    void startServerPoll()
    {
      (new Thread() {
	public void run()
	{
	  while(!ready)
	  {
	    try {
	      sleep(2000);
	      mds_server = new MdsServer(ip_address, useJavaServer, watchdogPort);
	      mds_server.addMdsServerListener(ActionServer.this);
	      mds_server.addConnectionListener(ActionServer.this);
	      System.out.println("Reconnected to to server " + ip_address + " server class " + server_class);
	      ready = active = true;
	      processConnected(ip_address);
	    }
	    catch (Exception exc){}
	  }
	}}).start();
    }
}



