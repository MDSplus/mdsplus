import java.util.*;

class jDispatcher implements ServerListener
{
    static boolean verbose = true;

    static final int MONITOR_BUILD_BEGIN = 1;
    static final int MONITOR_BUILD = 2;
    static final int MONITOR_BUILD_END = 3;
    static final int MONITOR_DISPATCHED = 5;
    static final int MONITOR_DOING = 6;
    static final int MONITOR_DONE = 7;
    
    boolean doing_phase = false;
    
    String tree;
    /**
    currently open tree
    */
    int shot;
    /**
    current shot;
    */
    
    int timestamp = 1;
    /**
    timestamp used for messages. Incremented each new seqeuence. Messages with older timestamp
    are discarded.
    */
    Vector seq_dispatched = new Vector();
    /**
    Vector of currently dispatched sequential actions
    */
    Vector dep_dispatched = new Vector();
    /**
    Vector of currently dispatched dependent actions
    */
    Enumeration curr_seq_numbers = null;
    /**
    Contains the sequence numbers of the current phase
    */
    
    
    static class PhaseDescriptor
    /** 
    PhaseDescriptor carries all the data structures required by each phase
    */
    {
        String phase_name;
        Vector seq_numbers = new Vector();
        /** 
        Active sequence numbers, in ascending order
        */
        Hashtable seq_actions = new Hashtable();
        /**
        Indexed by sequence number. For each sequence number, a vector of actions is defined.
        */
        Hashtable dependencies = new Hashtable();
        /**
        Indexed by action data (not actions!). For each action, a vector of potentially dependent actions is defined.
        Note that the hierarchy of ActionData does not override equals and hashCode, and therefore
        two actions are equal only if ther refer to the same instance.
        */
        PhaseDescriptor(String phase_name)
        {
            this.phase_name = phase_name;
        }
    }
    protected PhaseDescriptor curr_phase; //selects the current phase data structures
    
    Hashtable phases = new Hashtable();
    /**
    Indexed by phase name. Associates dispatch data structures with each phase.
    */
    
    Hashtable actions = new Hashtable();
    /**
    Indexed by ActionData, used to retrieve actions from ActionData in method isEnabled
    */
    
    Hashtable action_nids = new Hashtable();
    /**
    Indexed by nid, used to retrieve actions to be manually dispatched 
    */
    
    protected Vector servers = new Vector();
    /** 
    server list, used for collecting dispatch information. Servers will receive work by the Balancer
    */
    protected Balancer balancer;
    /** 
    All actions will be dispatched to balancer
    */

    protected Vector monitors = new Vector();
    /**
    Registered Monitor listeners.
    */
    
    public jDispatcher(Balancer balancer) {this.balancer = balancer; }
    
    public synchronized void clearTables()
    {
        actions.clear();
        action_nids.clear();
        phases.clear();
        dep_dispatched.removeAllElements();
        seq_dispatched.removeAllElements();
        timestamp++;
    }
   
   
    public synchronized void addServer(Server server)
    {
        servers.addElement(server);
        server.addServerListener(this);
        balancer.addServer(server);
    }


    public void setTree(String tree)
    {
        this.tree = tree;
        Enumeration server_list = servers.elements();
        while(server_list.hasMoreElements())
            ((Server)server_list.nextElement()).setTree(tree);
    }
        
        
    public synchronized void collectDispatchInformation()
    /**
    request actions to each server and insert them into hashtables
    */
    {
        clearTables();
        //fireMonitorEvent(null, MONITOR_BUILD_BEGIN);
        Enumeration server_list = servers.elements();
        while(server_list.hasMoreElements())
        {
            Server curr_server = (Server)server_list.nextElement();
            Action [] curr_actions = curr_server.collectActions();
            if(curr_actions != null)
            {
                for(int i = 0; i < curr_actions.length; i++)
                    insertAction(curr_actions[i], i == 0, i == curr_actions.length - 1);
            }
        }
        buildDependencies();
        //fireMonitorEvent(null, MONITOR_BUILD_END);
    }
    
    protected void insertAction(Action action, boolean is_first, boolean is_last)
    {
        //record current timestamp
        action.setTimestamp(timestamp);
        
        //Insert action in actions hashtable
        actions.put(action.getAction(), action);
        
        //Insert action in action_nids hashtable
        action_nids.put(new Integer(action.getNid()), action);
        
        //Check if the Action is sequential
        DispatchData dispatch = (DispatchData)action.getAction().getDispatch();
        if(dispatch == null)
        {
            System.out.println("Warning: Action "+action+" without dispatch info");
            return;
        }
        else
        {
            String phase_name, ident;

            try {
                phase_name = dispatch.getPhase().getString().toUpperCase();
            }
            catch(Exception exc)
            {
                System.out.println("Warning: Action "+action+" does not contain a phase string");
                return;
            }
            try {
                ident = dispatch.getIdent().getString();
            }
            catch(Exception exc)
            {
                System.out.println("Warning: Action "+action+" does not containg a server class string");
                return;
            }
                
            curr_phase = (PhaseDescriptor)phases.get(phase_name);
            if(curr_phase == null)
            {
                curr_phase = new PhaseDescriptor(phase_name);
                phases.put(phase_name, curr_phase);
            }
            if(dispatch.getType() == DispatchData.SCHED_SEQ)
            {
                int seq_number;
                try {
                    seq_number = getInt(dispatch.getWhen());
                }catch(Exception exc) 
                {
                System.out.println("Warning: expression used for sequence number");
                return;
                }
                Integer seq_obj = new Integer(seq_number);
                if(curr_phase.seq_actions.containsKey(seq_obj))
		{   
                    ((Vector)curr_phase.seq_actions.get(seq_obj)).addElement(action);
		}
                else //it is the first time such a sequence number is referenced
                {
                    Vector curr_vector = new Vector();
                    curr_vector.add(action);
                    curr_phase.seq_actions.put(seq_obj, curr_vector);
                    int size = curr_phase.seq_numbers.size();
                    if(seq_number >= size)
                    {
                        for(int i = size; i < seq_number; i++)
                            curr_phase.seq_numbers.addElement(new Integer(-1));
                        curr_phase.seq_numbers.addElement(seq_obj);
                    }
                    else
                    {
                        if(((Integer)curr_phase.seq_numbers.elementAt(seq_number)).intValue() == -1)
                        {
                            curr_phase.seq_numbers.removeElementAt(seq_number);
                            curr_phase.seq_numbers.insertElementAt(seq_obj, seq_number);
                        }
                    }
                }
            }
            curr_phase.dependencies.put(action.getAction(), new Vector());  //Insert every new action in dependencies hashtable
            if(is_first)
                fireMonitorEvent(action, MONITOR_BUILD_BEGIN);
            else if (is_last)
                fireMonitorEvent(action, MONITOR_BUILD_END);
            else
                fireMonitorEvent(action, MONITOR_BUILD);
        }
    }
    
    protected void buildDependencies()
    {
        DispatchData dispatch;
        if(curr_phase == null) return;
        Enumeration action_list = curr_phase.dependencies.keys();
        while(action_list.hasMoreElements())
        {
            ActionData action_data = (ActionData)action_list.nextElement();
            try {
                dispatch = (DispatchData)action_data.getDispatch();
            }catch (Exception e)
            {  continue; }
            if(dispatch.getType() == DispatchData.SCHED_COND)
                traverseDispatch(action_data, dispatch.getWhen());
        }
    }
    
    protected void traverseDispatch(ActionData action_data, Data when)
    {
        Action action;
        if(when == null) return;
        if(when instanceof ConditionData)
        {
            Vector act_vector = (Vector)curr_phase.dependencies.get(((ConditionData)when).getArgument());
            if (act_vector != null && (action = (Action)actions.get(action_data)) != null)
                act_vector.addElement(action);
            else
                System.out.println("Warning: condition does not refer to a known action");
        }
        else if(when instanceof ActionData)
        {
            Vector act_vector = (Vector)curr_phase.dependencies.get(when);
            if (act_vector != null && (action = (Action)actions.get(action_data)) != null)
                act_vector.addElement(action);
            else
                System.out.println("Warning: condition does not refer to a known action");
        }
        else if(when instanceof DependencyData)
        {
            Data[] args = ((DependencyData)when).getArguments();
            for(int i = 0; i < args.length; i++)
                traverseDispatch(action_data, args[i]);
        }
    }
    
        
   protected int getInt(Data data)
   /** 
   Returns an integer without evaluating expressions. This method will be overridden by 
   derived classes to achieve TDI expression evaluation ofr non pure Java dispatchers
   */
   {
        try {
            return data.getInt();
        }catch(Exception exc) 
        {
            System.out.println("Error: expressions not supported for sequence numbers");
            return 0;
        }
   }
    
     
   public synchronized void addMonitorListener(MonitorListener monitor)
   {
        monitors.addElement(monitor);
   }
   
   protected synchronized void fireMonitorEvent(Action action, int mode)
   {
        String server;

        MonitorEvent event = new MonitorEvent(this, tree, shot, (curr_phase == null)?"NONE":curr_phase.phase_name, action);
        Enumeration monitor_list = monitors.elements();
        while(monitor_list.hasMoreElements())
        {   
            MonitorListener curr_listener = (MonitorListener)monitor_list.nextElement();
            switch(mode) {
                case MONITOR_BUILD_BEGIN: curr_listener.buildBegin(event); break;
                case MONITOR_BUILD: curr_listener.build(event); break;
                case MONITOR_BUILD_END: curr_listener.buildEnd(event); break;
                case MONITOR_DISPATCHED: curr_listener.dispatched(event); break;
                case MONITOR_DOING: curr_listener.doing(event); break;
                case MONITOR_DONE: curr_listener.done(event); break;
            }
        }
   }
   
   public synchronized boolean startPhase(String phase_name)
   {
        doing_phase = false;
        //increment timestamp. Incoming messages with older timestamp will be ignored
        curr_phase = (PhaseDescriptor)phases.get(phase_name); //select data structures for the current phase
        if(curr_phase == null) return false; //Phase name does not correspond to any known phase.
        curr_seq_numbers = curr_phase.seq_numbers.elements(); 
        //curr_seq_numbers contains the sequence number for the selected phase
        
        if(curr_seq_numbers.hasMoreElements())
        {
            Integer curr_int;
            do {
                curr_int = (Integer)curr_seq_numbers.nextElement();
            }while(curr_int.intValue() == -1);
            Vector first_action_vect = (Vector)curr_phase.seq_actions.get(curr_int);
            Enumeration first_actions = first_action_vect.elements();
            while(first_actions.hasMoreElements())
            {
                Action action = (Action)first_actions.nextElement();
                if(action.isOn())
                {
                    doing_phase = true;
                    seq_dispatched.addElement(action);
                    action.setStatus(Action.DISPATCHED, 0, verbose);
                    balancer.enqueueAction(action);
                    fireMonitorEvent(action, MONITOR_DISPATCHED);

                }
            }
            return true;
        }
        return false; //no actions to be executed in this phase
   }
      
    public synchronized boolean dispatchAction(int nid)
    {
        if(action_nids == null) return false;
        Action action = (Action)action_nids.get(new Integer(nid));
        if(action == null) return false;
        action.setStatus(Action.DISPATCHED, 0, verbose);
        action.setManual(true);
        balancer.enqueueAction(action);
        fireMonitorEvent(action, MONITOR_DISPATCHED);
        return true;
    }
     
      
    public synchronized void abortAction(int nid)
    {
        if(action_nids == null) return;
        Action action = (Action)action_nids.get(new Integer(nid));
        if(action == null) return;
        balancer.abortAction(action);
    }
        
    public synchronized void waitPhase()
    {
        try {
            while(doing_phase)
                wait();
        }catch(InterruptedException exc){}
    }
        
        
        
    public synchronized void actionStarting(ServerEvent event) 
    /**
    called by a server to notify that the action is starting being executed.
    Simply reports the fact
    */
    {
        //if(event.getTimestamp() != timestamp) //outdated message
         //   return;
        event.getAction().setStatus(Action.DOING, 0, verbose);
        fireMonitorEvent(event.getAction(), MONITOR_DOING);

    }
    
    public synchronized void actionAborted(ServerEvent event) 
    /**
    called by a server to notify that the action is starting being executed.
    Simply reports the fact
    */
    {
//        if(event.getTimestamp() != timestamp) //outdated message
//            return;
        event.getAction().setStatus(Action.ABORTED, 0, verbose);
        fireMonitorEvent(event.getAction(), MONITOR_DONE);
        reportDone(event.getAction());
   }
    
    
    public synchronized void actionFinished(ServerEvent event)
    /**
    called by a server to notify that the action has finished 
    */
    {
       // if(event.getTimestamp() != timestamp) //outdated message
       //     return;
        //update status in report
        Action action = event.getAction();
        try {
            String mdsevent = ((StringData)(((DispatchData)(action.getAction().getDispatch())).getCompletion())).getString();
            if(mdsevent != null && !mdsevent.equals("\"\""))
            {
                MdsHelper.generateEvent(mdsevent, 0);
            }
        }catch(Exception exc){}
     
        action.setStatus(Action.DONE, event.getStatus(), verbose);
        fireMonitorEvent(action, MONITOR_DONE);
        if(!action.isManual())
            reportDone(action);
        else
            action.setManual(false);
    }
        
   protected void reportDone(Action action)
   {
        //remove action from dispatched
        if(!seq_dispatched.removeElement(action))
            dep_dispatched.removeElement(action); //The action belongs only to one of the two
        //check dependent actions
        try {
            Enumeration depend_actions = ((Vector)curr_phase.dependencies.get(action.getAction())).elements();
            while(depend_actions.hasMoreElements())
            {
                Action curr_action = (Action)(depend_actions.nextElement());
                if(curr_action.isOn() && isEnabled(((DispatchData)curr_action.getAction().getDispatch()).getWhen())) //the dependent action is now enabled
                {
                    dep_dispatched.addElement(curr_action);
                    curr_action.setStatus(Action.DISPATCHED, 0, verbose);
                    balancer.enqueueAction(curr_action);
                    fireMonitorEvent(curr_action, MONITOR_DISPATCHED);
               }
            }
        }catch(Exception exc) {System.err.println("Internal error: action not stored in depend_actions hashtable");}
        if(seq_dispatched.isEmpty())  //No more sequential actions for this sequence number 
        {
            if(curr_seq_numbers.hasMoreElements()) //Still further sequence numbers
            {
                Integer curr_int;
                do {
                    curr_int = (Integer)curr_seq_numbers.nextElement();
                }while(curr_int.intValue() == -1);
                
                Vector first_action_vect = (Vector)curr_phase.seq_actions.get(curr_int);
                Enumeration actions = first_action_vect.elements();
                while(actions.hasMoreElements())
                {
                    Action curr_action = (Action)actions.nextElement();
                    seq_dispatched.addElement(curr_action);
		    curr_action.setStatus(Action.DISPATCHED, 0, verbose); //Spostata da cesare
                    balancer.enqueueAction(curr_action);
//                    curr_action.setStatus(Action.DISPATCHED, 0, verbose);
                    fireMonitorEvent(curr_action, MONITOR_DISPATCHED);
               }
            }
            else
            {
                if(dep_dispatched.isEmpty()) //No more actions at all for this phase
                {
                    doing_phase = false;
                    notify();
                }
            }
        }
    }
                
            
    protected boolean isEnabled(Data when)
    /**
    Check whether this action is enabled to execute, based on the current status hold 
    in curr_status.reports hashtable.
    In this class the check is done in Java. In a derived class it may be performed by evaluating
    a TDI expression.
    */
    {
        if(when instanceof ConditionData)
        {
            int modifier = ((ConditionData)when).getModifier();
            Action action = (Action)actions.get(((ConditionData)when).getArgument());
            int dispatch_status = action.getDispatchStatus();
            int status = action.getStatus();
            if(dispatch_status != Action.DONE) return false;
            switch (modifier) {
                case 0: if((status & 1) != 0) return true; return false;
                case ConditionData.IGNORE_UNDEFINED: //???
                case ConditionData.IGNORE_STATUS: return true; 
                case ConditionData.NEGATE_CONDITION: if((status & 1) == 0) return true; return false;
            }            
        }
        if(when instanceof ActionData)
        {
            Action action = (Action)actions.get(when);
            if(action.getDispatchStatus() != Action.DONE) return false;
        }
        if(when instanceof DependencyData)
        {
            Data args[] = ((DependencyData)when).getArguments();
            if(args.length != 2) 
            {
                System.out.println("Error: dependency needs 2 arguments. Ignored");
                return false;
            }
            int opcode = ((DependencyData)when).getOpcode();
            switch(opcode) {
                case DependencyData.DEPENDENCY_AND: return isEnabled(args[0]) && isEnabled(args[1]);
                case DependencyData.DEPENDENCY_OR: return isEnabled(args[0]) || isEnabled(args[1]);
            }
        }
        return true;
    }
    
            
    public synchronized void beginSequence(int shot)
    {
        this.shot = shot;
        Enumeration server_list = servers.elements();
        while(server_list.hasMoreElements())
            ((Server)server_list.nextElement()).beginSequence(shot);
    }
    public synchronized void endSequence(int shot)
    {
        Enumeration server_list = servers.elements();
        while(server_list.hasMoreElements())
            ((Server)server_list.nextElement()).endSequence(shot);
        clearTables();
    }


    public synchronized void abortPhase()
    {
        Action action;
        
        balancer.abort();
        while(!seq_dispatched.isEmpty())
        {
            action = (Action)seq_dispatched.elementAt(0);
            action.setStatus(Action.ABORTED, 0, verbose);
            seq_dispatched.removeElementAt(0);
        }
        while(!dep_dispatched.isEmpty())
        {
            action = (Action)dep_dispatched.elementAt(0);
            action.setStatus(Action.ABORTED, 0, verbose);
            seq_dispatched.removeElementAt(0);
        }
    } 
    public void setDefaultServer(Server server)
    {
        balancer.setDefaultServer(server);
    }

    public static void main(String args[])
    {
        Server server;
        Balancer balancer = new Balancer();
        jDispatcher dispatcher = new jDispatcher(balancer);
        dispatcher.addServer(new InfoServer("disp_test"));
        dispatcher.addServer(server = new ActionServer("disp_test", "150.178.3.47:8002", "server_1"));
        dispatcher.addServer(new ActionServer("disp_test", "150.178.3.47:8004", "server_1"));
        dispatcher.addServer(new ActionServer("disp_test", "150.178.3.47:8005", "server_2"));
        dispatcher.addServer(new ActionServer("disp_test", "150.178.3.47:8006", "server_2"));
        dispatcher.addServer(new ActionServer("disp_test", "150.178.3.47:8007", "server_3"));
        dispatcher.addServer(new ActionServer("disp_test", "150.178.3.47:8008", "server_3"));
        balancer.setDefaultServer(server);
        dispatcher.collectDispatchInformation();
        dispatcher.beginSequence(2);
        dispatcher.startPhase("INITIALIZATION");
        dispatcher.waitPhase();
        dispatcher.endSequence(2);
        System.exit(0);
    }


}
