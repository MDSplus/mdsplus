import java.util.*;
import javax.swing.*;
import java.awt.event.*;

class ActionServer implements Server, MdsServerListener, ConnectionListener
{
    Vector enqueued_actions = new Vector();
    Hashtable doing_actions = new Hashtable();
    Vector server_listeners = new Vector();
    Hashtable actions = new Hashtable();
    MdsServer mds_server = null;
    String tree;
    String server_class;
    String ip_address;
    int shot;
    javax.swing.Timer timer;
    static final int RECONNECT_TIME = 2; 
    
    
    public ActionServer(String tree, String ip_address, String server_class)
    {
        this.tree = tree;
        this.server_class = server_class;
        this.ip_address = ip_address;
        try {
            mds_server = new MdsServer(ip_address);
            mds_server.addMdsServerListener(this);
            mds_server.addConnectionListener(this);
        }catch(Exception exc) {mds_server = null; }
        timer = new javax.swing.Timer(0, new ActionListener() {
            public void actionPerformed(ActionEvent e)
            {   
                synchronized(ActionServer.this)
                {
                    timer.stop();
                    if(doing_actions.size() != 1) return; //handle only the usual case with 1 doing action
                    int queue_len = 0;
                    int size = enqueued_actions.size();
                    Vector removed_actions = new Vector();
                    Action curr_action;
                    while((curr_action = popAction()) != null)
                    {
                        removed_actions.addElement(curr_action);
                        queue_len++;
                    }
                    if(queue_len == size)
                    {
                        doing_actions.clear();
                        try {
                            mds_server.abort(false); //now we are sure that no other action gets aborted
                        } catch(Exception exc){}
                    }
                    Enumeration action_list = removed_actions.elements();
                    while(action_list.hasMoreElements())
                    {
                        Action action = (Action)action_list.nextElement();
                        try {
                            mds_server.dispatchAction(ActionServer.this.tree, shot, action.getNid(), action.getNid());
                            enqueued_actions.addElement(action);
                        }catch(Exception exc) {}
                    }
                }
            }});
            timer.setRepeats(false);
    }
    public void setTree(String tree) {this.tree = tree; }   
    public boolean isActive() {return mds_server != null; }
    public void processConnectionEvent(ConnectionEvent e)
    {
        if(e.getID() == ConnectionEvent.LOST_CONNECTION)
        {
            try {
                if(mds_server != null)
                    mds_server.shutdown();
            }catch(Exception exc) 
            {
                System.err.println("Error shutting down socket");
            }
            System.out.println("Detected server crash");
            javax.swing.Timer conn_timer = new javax.swing.Timer(RECONNECT_TIME * 1000, new ActionListener()
                {
                    public void actionPerformed(ActionEvent e)
                    {
                        synchronized(ActionServer.this)
                        {
                            try {
                                mds_server = new MdsServer(ip_address);
                                mds_server.addMdsServerListener(ActionServer.this);
                                mds_server.addConnectionListener(ActionServer.this);
                                mds_server.dispatchCommand("TCL", "SET TREE " + tree + "/SHOT=" + shot);
                                System.out.println("Restarting server");
                                Thread.currentThread().sleep(1000); //Give time to mdsip server to start its own threads
                            }catch(Exception exc) {mds_server = null; }
                            if(doing_actions.size() > 0)
                            {
                                Enumeration doing_list = doing_actions.elements();
                                while(doing_list.hasMoreElements())
                                    processAborted((Action)doing_list.nextElement()); //in any case aborts action currently being executed
                                doing_actions.clear();
                            }
                            Action action;
                            if(mds_server == null)
                            {
                                synchronized(enqueued_actions)
                                {
                                    if(enqueued_actions.size() > 0)
                                    {
                                        while(enqueued_actions.size() > 0 && (action = (Action)enqueued_actions.elementAt(0)) != null)
                                        {
                                            processAborted(action);
                                            enqueued_actions.removeElementAt(0);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                synchronized(enqueued_actions)
                                {
                                    for(int i = 0; i < enqueued_actions.size(); i++)
                                    {
                                        action = (Action)enqueued_actions.elementAt(i);
                                        try {
                                            mds_server.dispatchAction(tree, shot, action.getNid(), action.getNid());
                                        }catch(Exception exc) {}
                                    }
                                }
                            }
                        }
                    }});
                    conn_timer.setRepeats(false);
                    conn_timer.setInitialDelay(RECONNECT_TIME * 1000);
                    conn_timer.start();
                    
        }
    }
                        
    
    public synchronized void addServerListener(ServerListener listener)
    {
        server_listeners.addElement(listener);
    }
   
    public void pushAction(Action action)
    {
        synchronized (enqueued_actions){
            enqueued_actions.addElement(action);
        }
        try {
           // mds_server.dispatchAction(tree, shot, action.getNid(), action.getTimestamp());
            mds_server.dispatchAction(tree, shot, action.getNid(), action.getNid());
        }catch(Exception exc) {processAborted(action);}
    }
    
    
    public synchronized void processMdsServerEvent(MdsServerEvent e)
    {
       int mode = e.getFlags();
       Action doing_action;
        switch(mode) {
            case MdsServerEvent.SrvJobSTARTING:
                synchronized(enqueued_actions)
                {
                    doing_actions.put(new Integer(e.getJobid()), 
                        doing_action = (Action)enqueued_actions.firstElement());
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
                    timer.setInitialDelay(timeout * 1000);
                    timer.start();
                }
                
              
                break;
            case MdsServerEvent.SrvJobFINISHED :
                timer.stop();
                if(e.getJobid() == 0) return; //SrvJobFINISHED messages are generated also by SrvCreatePulse and SrvClose
                Action done_action = (Action)doing_actions.remove(new Integer(e.getJobid()));
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
                timer.stop();
                Action aborted_action = (Action)doing_actions.remove(new Integer(e.getJobid()));
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
        Enumeration listeners = server_listeners.elements();
        while(listeners.hasMoreElements())
        {
            ServerListener listener = (ServerListener)listeners.nextElement();
            listener.actionFinished(new ServerEvent(this, action));
        }
    }
    protected void processDoing(Action action)
    {
        
        
        Enumeration listeners = server_listeners.elements();
        while(listeners.hasMoreElements())
        {
            ServerListener listener = (ServerListener)listeners.nextElement();
            
            listener.actionStarting(new ServerEvent(this, action));
        }
    }
    protected synchronized void processAborted(Action action)
    {
        Enumeration listeners = server_listeners.elements();
        while(listeners.hasMoreElements())
        {
            ServerListener listener = (ServerListener)listeners.nextElement();
            listener.actionAborted(new ServerEvent(this, action));
            notify();
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
                Action removed = (Action)enqueued_actions.lastElement();
                enqueued_actions.removeElement(removed);
                return removed;
           }
        }
        return null;
    }
        
    
    public synchronized Action[] collectActions() {return null; }

    public void beginSequence(int shot)
    {
        this.shot = shot;
        if(mds_server == null) return;
        try {
            //mds_server.createPulse(tree, shot);
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

    public synchronized void abort(boolean flush)   
    {
        if(mds_server == null) return;
        try {
            mds_server.abort(flush);
        }catch(Exception exc){}
    }
    
    public synchronized boolean abortAction(Action action)
    {
        if((doing_actions.get(new Integer(action.getNid())) == null) && enqueued_actions.indexOf(action) == -1)
            return false;
        int queue_len = 0;
        synchronized(enqueued_actions)
        {
            int size = enqueued_actions.size();
            Vector removed_actions = new Vector();
            Action curr_action;
            while((curr_action = popAction()) != null)
            {
                removed_actions.addElement(curr_action);
                queue_len++;
            }
            int abort_idx = removed_actions.indexOf(action);
            if(abort_idx != -1) //the action was waiting
                removed_actions.removeElementAt(abort_idx);
            else //the action was executing, most likely option
            {
                try{
                    mds_server.abort(false); //now we are sure that no other action gets aborted
                } catch(Exception exc){}
            }
            Enumeration action_list = removed_actions.elements();
            while(action_list.hasMoreElements())
            {
                curr_action = (Action)action_list.nextElement();
                try {
                    mds_server.dispatchAction(ActionServer.this.tree, shot, 
                        curr_action.getNid(), curr_action.getNid());
                    enqueued_actions.addElement(curr_action);
                }catch(Exception exc) {}
            }
        }
        return true;
    }
}        
    
     
    
    