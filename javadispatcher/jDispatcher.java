import java.util.*;
import java.io.*;
import java.net.*;

class jDispatcher
    implements ServerListener
{
    static boolean verbose = true;

    static final int MONITOR_BEGIN_SEQUENCE = 1;
    static final int MONITOR_BUILD_BEGIN = 2;
    static final int MONITOR_BUILD = 3;
    static final int MONITOR_BUILD_END = 4;
    static final int MONITOR_CHECKIN = 5;
    static final int MONITOR_DISPATCHED = 6;
    static final int MONITOR_DOING = 7;
    static final int MONITOR_DONE = 8;
    static final int MONITOR_START_PHASE = 9;
    static final int MONITOR_END_PHASE = 10;
    static final int MONITOR_END_SEQUENCE = 11;


    static final int TdiUNKNOWN_VAR = 0xfd380f2;

    boolean doing_phase = false;

    Hashtable<String, Vector<Integer>> synchSeqNumberH = new Hashtable<String, Vector<Integer>>();
    //Synch number sequence for every phase name

    Vector<Integer> synchSeqNumbers = new Vector<Integer>();
    /**
     * synchSeqNumbers contains the synchronization sequenc numbers defined in jDispatcher.properties.
     *
     */
    String defaultServerName = "";


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
    Hashtable<String, Vector<Action>> totSeqDispatched = new Hashtable<String, Vector<Action>>();
    //Indexed by server class, every element of totSeqDispatched is the vector of currently dispatched
    //action for a given server class

    //OLD Vector seq_dispatched = new Vector();
    /**
         Vector of currently dispatched sequential actions
     */
    Vector<Action> dep_dispatched = new Vector<Action>();
    /**
         Vector of currently dispatched dependent actions
     */
    Hashtable<String, Enumeration<Integer>> totSeqNumbers = new Hashtable<String, Enumeration<Integer>>();
    //Indexed by server class, every element of totSeqNumber contains the enumeration
    //of the sequence numbers for this phase and this server class
    Hashtable<String, Integer> actSeqNumbers = new Hashtable<String, Integer>();
    //Indexed by server class. Keeps track of the current sequence number;
    Hashtable<String, Boolean> phaseTerminated = new Hashtable<String, Boolean>();
    //Indexed by server class. Keeps track of whether the curent phase has terminated for this server class

    //OLD Enumeration curr_seq_numbers = null;
    /**
         Contains the sequence numbers of the current phase
     */

    Hashtable<Integer, Vector<Action>> nidDependencies = new Hashtable<Integer, Vector<Action>>();
    /**
             Indexed by nid. For each nid, a vector of potentially dependent actions is defined.
             Note that the hierarchy of ActionData does not override equals and hashCode, and therefore
             two actions are equal only if they refer to the same instance.
             This kind of dependency is NOT restricted to the current phase.
     */

    static class PhaseDescriptor
    /**
         PhaseDescriptor carries all the data structures required by each phase
     */
    {
        String phase_name;
        Hashtable<String, Vector<Integer>> totSeqNumbers = new Hashtable<String, Vector<Integer>>();
        //Indexed by server class, every element of totSeqNumbers contains the sequence number for this
        //phase and the given server class
        //OLD Vector seq_numbers = new Vector();
        /**
                 Active sequence numbers, in ascending order
         */
        Hashtable<String, Hashtable<Integer, Vector<Action>>> totSeqActions = new Hashtable<String, Hashtable<Integer, Vector<Action>>>();
        //Indexed by server class, every element of totSeqActions is in turn an Hashtable indexed by sequence
        //number, associating a vector of actions (i.e. the actions for the given sequence number and
        //for the given server class in this phase
        // OLD Hashtable seq_actions = new Hashtable();
        /**
                 Indexed by sequence number. For each sequence number, a vector of actions is defined.
         */
        Hashtable<ActionData, Vector<Action>> dependencies = new Hashtable<ActionData, Vector<Action>>();
        /**
                 Indexed by action data (not actions!). For each action, a vector of potentially dependent actions is defined.
                 Note that the hierarchy of ActionData does not override equals and hashCode, and therefore
                 two actions are equal only if ther refer to the same instance.
         */


        Vector<Action> immediate_actions = new Vector<Action>();
        /**
                 List of conditional actions with no dependency. These actions will be started immediately
                 at the beginning of the phase
         */

        Hashtable<Integer, Action> all_actions = new Hashtable<Integer, Action>();
        /**
         A complete list of actions for that phase, indexed by their nid
         */

        PhaseDescriptor(String phase_name)
        {
            this.phase_name = phase_name;
        }
    }

    public jDispatcher()
    {
    }

    protected PhaseDescriptor curr_phase; //selects the current phase data structures

    Hashtable<String, PhaseDescriptor> phases = new Hashtable<String, PhaseDescriptor>();
    /**
     Indexed by phase name. Associates dispatch data structures with each phase.
     */

    Hashtable<ActionData, Action> actions = new Hashtable<ActionData, Action>();
    /**
         Indexed by ActionData, used to retrieve actions from ActionData in method isEnabled
     */

    Hashtable<Integer, Action> action_nids = new Hashtable<Integer, Action>();
    /**
         Indexed by nid, used to retrieve actions to be manually dispatched
     */

    protected Vector<Server> servers = new Vector<Server>();
    /**
         server list, used for collecting dispatch information. Servers will receive work by the Balancer
     */
    protected Balancer balancer;
    /**
         All actions will be dispatched to balancer
     */

    protected Vector<MonitorListener> monitors = new Vector<MonitorListener>();
    /**
         Registered Monitor listeners.
     */

    public jDispatcher(Balancer balancer)
    {
        this.balancer = balancer;
    }

    public synchronized void clearTables()
    {
        actions.clear();
        action_nids.clear();
        phases.clear();
        dep_dispatched.removeAllElements();
        totSeqDispatched = new Hashtable<String, Vector<Action>>();
        actSeqNumbers = new Hashtable<String, Integer>();
        //seq_dispatched.removeAllElements();
        nidDependencies.clear();
        timestamp++;
    }

    public boolean checkEssential()
    {
         Enumeration<Action> actionsEn = curr_phase.all_actions.elements();
        while(actionsEn.hasMoreElements())
        {
            Action action = actionsEn.nextElement();
            if(action.isEssential() && (action.getDispatchStatus() != Action.DONE || ((action.getStatus() & 1) == 0)))
                return false;
        }
        return true;
    }

    int getFirstValidSynch()
    {
        //return the first synch number greater than or equal to any sequence number
        for(int idx = 0; idx < synchSeqNumbers.size(); idx++)
        {
            int currSynch = synchSeqNumbers.elementAt(idx);
            Enumeration serverClasses = curr_phase.totSeqNumbers.keys();
            while(serverClasses.hasMoreElements())
            {
                String currServerClass = (String)serverClasses.nextElement();
                Vector<Integer> currSeqVect = curr_phase.totSeqNumbers.get(currServerClass);
                if(currSeqVect.size() > 0)
                {
                    if(currSeqVect.elementAt(0) <= currSynch)
                        return currSynch;
                }
            }
        }
        return -1;
     }


    //Check which servers  can proceede to the next sequence number
    //Either None, or this server or a list of server classes
    Vector<String> canProceede(String serverClass)
    {
        //Find the most advanced sequence number
        Enumeration<Integer> seqNumbers = actSeqNumbers.elements();
        int maxSeq = -1;
        while(seqNumbers.hasMoreElements())
        {
            int currSeq = seqNumbers.nextElement();
            if(currSeq > maxSeq)
                maxSeq = currSeq;
        }
        //Find smallest synch number which is greater or equan to maxSeq
        int actSynch = -1;
        int nextSynch = -1;
        for(int idx = 0; idx < synchSeqNumbers.size(); idx++)
        {
            int currSynch = synchSeqNumbers.elementAt(idx);
            if(currSynch >= maxSeq)
            {
                actSynch = currSynch;
                if(idx == synchSeqNumbers.size() - 1)
                    nextSynch = actSynch;
                else
                    nextSynch = synchSeqNumbers.elementAt(idx+1);
                break;
            }
         }
         if(actSynch == -1) //No more synch numbers, proceede freely
         {
             Vector<String> retVect = new Vector<String>();
             retVect.addElement(serverClass);
             return retVect;
         }
  //If the next sequence number is less than or equal to actSynch. it can proceede.
         Integer thisSeq = actSeqNumbers.get(serverClass);
         Vector<Integer> currSeqNumbers = curr_phase.totSeqNumbers.get(serverClass);
         int thisIdx = currSeqNumbers.indexOf(thisSeq);
         //if it is the last element can trivially proceede (it does nothing)
         int thisSeqN = thisSeq.intValue();
         if(thisIdx == currSeqNumbers.size()-1 && thisSeqN != actSynch)
         {
             Vector<String> retVect = new Vector<String>();
             retVect.addElement(serverClass);
             return retVect;
         }
         if(thisIdx < currSeqNumbers.size()-1 )
         {
            if (currSeqNumbers.elementAt(thisIdx + 1) <= actSynch) {
                Vector<String> retVect = new Vector<String>();
                retVect.addElement(serverClass);
                return retVect;
            }
         }
  //Otherwise we must check that all the servers have reached a condition where they have
  //either finished or the next sequence number is larger that actSynch
          //In any case wait until all dispatched actions for any server
          if(!allSeqDispatchedAreEmpty())
              return new Vector<String>();


         Enumeration<String> serverClasses = curr_phase.totSeqNumbers.keys();
         Vector<String> serverClassesV = new Vector<String>();
         while(serverClasses.hasMoreElements())
         {
            String currServerClass = serverClasses.nextElement();
            Integer currSeqNum = actSeqNumbers.get(currServerClass);
            Vector<Integer> currSeqVect = curr_phase.totSeqNumbers.get(currServerClass);
            int currSeqN = currSeqNum.intValue();
            if(currSeqN == -1) //This server has not yet started
            {
                int firstSeqN = currSeqVect.elementAt(0);
                if((nextSynch == actSynch && firstSeqN > actSynch) || //If the lase synch number
                        (nextSynch > actSynch && firstSeqN <= nextSynch)) //or before the next
                    serverClassesV.addElement(currServerClass);
                //Will start only if the first sequence number is greater than the curent synch step
            }
            else
            {
                int currIdx = currSeqVect.indexOf(currSeqNum);
         //if it is the last element can trivially proceede (it does nothing)
                serverClassesV.addElement(currServerClass);
                if(currIdx < currSeqVect.size()-1) //It is the last element of the sequence, skip it
                {
                    if (currSeqVect.elementAt(currIdx + 1) >= actSynch)
                        return new Vector<String>(); //Empty array
            //There is at least one server class which has not yet
            // reached the synchronization number
                }
            }
         }
         //Return the array of all server names
         return serverClassesV; //If I arrive here, all the servers are ready to pass to the next synch step
    }


    boolean allSeqDispatchedAreEmpty()
    {
        Enumeration serverClasses = totSeqDispatched.keys();
        while(serverClasses.hasMoreElements())
        {
            String serverClass = (String)serverClasses.nextElement();
            if (!totSeqDispatched.get(serverClass).isEmpty());
                return false;
        }
        return true;
    }

    boolean isConditional(Action action)
    {
        DispatchData dispatch = (DispatchData) action.getAction().getDispatch();
        if(dispatch.getType() != DispatchData.SCHED_SEQ )
            return true;
         if((dispatch.getWhen() instanceof NidData) || (dispatch.getWhen() instanceof PathData)
             || (dispatch.getWhen() instanceof IdentData))
                return true;
        return false;

    }

    boolean allTerminatedInPhase()
    {
        Enumeration serverClasses = phaseTerminated.keys();
        while(serverClasses.hasMoreElements())
        {
            String serverClass = (String)serverClasses.nextElement();
            if(!(phaseTerminated.get(serverClass)).booleanValue())
                return false;
        }
        return true;
    }

    public synchronized void addSynchNumbers(String phase, Vector<Integer> synchNumbers)
    {
        synchSeqNumberH.put(phase, synchNumbers);
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
        while (server_list.hasMoreElements())
            ( (Server) server_list.nextElement()).setTree(tree);
    }

    public void setTree(String tree, int shot)
    {
	Database mdsTree = new Database(tree, shot);
	try {
		mdsTree.open();
	}catch(Exception exc) {System.err.println("Cannot open tree " + tree + " " + shot);}
        this.tree = tree;
        this.shot = shot;
        Enumeration server_list = servers.elements();
        while (server_list.hasMoreElements())
            ( (Server) server_list.nextElement()).setTree(tree, shot);
    }

    public synchronized void collectDispatchInformation()
    /**
         request actions to each server and insert them into hashtables
     */
    {
        clearTables();
        //fireMonitorEvent(null, MONITOR_BUILD_BEGIN);
        Enumeration server_list = servers.elements();
        while (server_list.hasMoreElements()) {
            Server curr_server = (Server) server_list.nextElement();
            Action[] curr_actions = curr_server.collectActions();
            if (curr_actions != null) {
                for (int i = 0; i < curr_actions.length; i++)
                    insertAction(curr_actions[i], i == 0,
                                 i == curr_actions.length - 1);
            }
        }
        buildDependencies();
        //fireMonitorEvent(null, MONITOR_BUILD_END);
    }
    public synchronized void collectDispatchInformation(String rootPath)
    /**
         request actions to each server and insert them into hashtables
     */
    {
        clearTables();
        //fireMonitorEvent(null, MONITOR_BUILD_BEGIN);
        Enumeration server_list = servers.elements();
        while (server_list.hasMoreElements()) {
            Server curr_server = (Server) server_list.nextElement();
            Action[] curr_actions = curr_server.collectActions(rootPath);
            if (curr_actions != null) {
                for (int i = 0; i < curr_actions.length; i++)
                    insertAction(curr_actions[i], i == 0,
                                 i == curr_actions.length - 1);
            }
        }
        buildDependencies();
        //fireMonitorEvent(null, MONITOR_BUILD_END);
    }

     protected String getServerClass(Action action)
    {
        try {
            DispatchData dispatch = (DispatchData)action.getAction().getDispatch();
            String serverClass = dispatch.getIdent().getString().toUpperCase();
            if (serverClass == null || serverClass.equals(""))
                return defaultServerName;
            return balancer.getActServer(serverClass);
        }catch(Exception exc){return defaultServerName;}
    }


    protected void insertAction(Action action, boolean is_first,
                                boolean is_last)
    {
        String serverClass = getServerClass(action);
        //record current timestamp
        action.setTimestamp(timestamp);

        //Insert action in actions hashtable
        actions.put(action.getAction(), action);

        //Insert action in action_nids hashtable
        action_nids.put(new Integer(action.getNid()), action);

        //Check if the Action is sequential
        DispatchData dispatch = (DispatchData) action.getAction().getDispatch();
        if (dispatch == null) {
            System.out.println("Warning: Action " + action +
                               " without dispatch info");
            return;
        }
        else {
            String phase_name, ident;

            try {
                phase_name = dispatch.getPhase().getString().toUpperCase();
            }
            catch (Exception exc) {
                System.out.println("Warning: Action " + action +
                                   " does not contain a phase string");
                return;
            }
            try {
                ident = dispatch.getIdent().getString();
            }
            catch (Exception exc) {
                System.out.println("Warning: Action " + action +
                                   " does not containg a server class string");
                return;
            }

            curr_phase = phases.get(phase_name);
            if (curr_phase == null) {
                curr_phase = new PhaseDescriptor(phase_name);
                phases.put(phase_name, curr_phase);
            }
            curr_phase.all_actions.put(new Integer(action.getNid()), action);
            boolean isSequenceNumber = true;
            if (dispatch.getType() == DispatchData.SCHED_SEQ) {
                int seq_number = 0;
                if(dispatch.getWhen() instanceof NidData)
                    isSequenceNumber = false;
                else
                {
                    try {
                        seq_number = dispatch.getWhen().getInt();
                    }
                    catch (Exception exc) {
                        isSequenceNumber = false;
//System.out.println("Warning: expression used for sequence number");
                    }
                }
                if(!isSequenceNumber)
                    dispatch.descs[2] =  traverseSeqExpression(action.getAction(), dispatch.getWhen());

                Hashtable<Integer, Vector<Action>> seqActions = curr_phase.totSeqActions.get(serverClass);
                if(seqActions == null)
                    curr_phase.totSeqActions.put(serverClass, seqActions = new Hashtable<Integer, Vector<Action>>());
                Vector<Integer> seqNumbers = curr_phase.totSeqNumbers.get(serverClass);
                if(seqNumbers == null)
                    curr_phase.totSeqNumbers.put(serverClass, seqNumbers = new Vector<Integer>());
                if (isSequenceNumber) {
                    Integer seq_obj = new Integer(seq_number);
                    if (seqActions.containsKey(seq_obj))
                        seqActions.get(seq_obj).addElement(action);
                    else {
                        //it is the first time such a sequence number is referenced
                        Vector<Action> curr_vector = new Vector<Action>();
                        curr_vector.add(action);
                        seqActions.put(seq_obj, curr_vector);

       /*//////////////////////////////////////////////////DA AGGIUNGERE ORDINATO!!!!!


                        int size = seqNumbers.size();
                        if (seq_number >= size) {
                            for (int i = size; i < seq_number; i++)
                                seqNumbers.addElement(new Integer(-1));
                            seqNumbers.addElement(seq_obj);
                        }
                        else {
                            if (seqNumbers.elementAt(seq_number) == -1) {
                                seqNumbers.removeElementAt(seq_number);
                                seqNumbers.insertElementAt(seq_obj, seq_number);
                            }
                        }

      ///////////////////////////////////////////////////////////////////////////////////*/
                         if(seqNumbers.size() == 0)
                            seqNumbers.addElement(seq_obj);
                        else
                        {
                            int idx, currNum = -1;
                            for(idx = 0; idx < seqNumbers.size(); idx++)
                            {
                                currNum = seqNumbers.elementAt(idx);
                                if(currNum >= seq_number)
                                    break;
                            }
                            if(currNum != seq_number)
                                seqNumbers.insertElementAt(seq_obj, idx);
                        }
                    }
                }
            }
//////////////////////////GAB CHRISTMAS 2004
            //Handle Conditional actions with no dependencies
            //these will be dispatched asynchronously at the beginning of the phase
            if (dispatch.getType() == DispatchData.SCHED_COND &&
                dispatch.getWhen() == null) {
                curr_phase.immediate_actions.addElement(action);
            }
//////////////////////////GAB CHRISTMAS 2440

            curr_phase.dependencies.put(action.getAction(), new Vector<Action>()); //Insert every new action in dependencies hashtable
            if (is_first)
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

        Enumeration phaseNames = phases.keys();
        while (phaseNames.hasMoreElements()) {
            PhaseDescriptor currPhase = phases.get(phaseNames.nextElement());
            if (currPhase == null)
                continue;
            Enumeration action_list = currPhase.dependencies.keys();
            while (action_list.hasMoreElements()) {
                ActionData action_data = (ActionData) action_list.nextElement();
                try {
                    dispatch = (DispatchData) action_data.getDispatch();
                }
                catch (Exception e) {
                    continue;
                }
                if (dispatch.getType() == DispatchData.SCHED_COND)
                    traverseDispatch(action_data, dispatch.getWhen(), currPhase);

            }
        }
    }

    protected void traverseDispatch(ActionData action_data, Data when,
                                    PhaseDescriptor currPhase)
    {
        Action action;
        if (when == null)
            return;
        if (when instanceof ConditionData) {
            Vector<Action> act_vector = currPhase.dependencies.get(((ConditionData)when).getArgument());
            if (act_vector != null &&
                (action = actions.get(action_data)) != null)
                act_vector.addElement(action);
            else
                System.out.println(
                    "Warning: condition does not refer to a known action");
        }
        else if (when instanceof ActionData) {
            Vector<Action> act_vector = currPhase.dependencies.get(when);
            if (act_vector != null &&
                (action = actions.get(action_data)) != null)
                act_vector.addElement(action);
            else
                System.out.println(
                    "Warning: condition does not refer to a known action");
        }
        else if (when instanceof DependencyData) {
            Data[] args = ( (DependencyData) when).getArguments();
            for (int i = 0; i < args.length; i++)
                traverseDispatch(action_data, args[i], currPhase);
        }
    }

    protected Data traverseSeqExpression(ActionData action_data, Data seq)
    {
        Action action = actions.get(action_data);
        if (action == null) {
            System.err.println(
                "Internal error in traverseSeqExpression: no action for action_data");
            return null;
        }
        Database tree = InfoServer.getDatabase();
        if (seq == null)
            return null;
        if (seq instanceof PathData || seq instanceof NidData) {
            int nid;
            try {

                if (seq instanceof PathData)
                    nid = (tree.resolve( (PathData) seq, 0)).getInt();
                else
                    nid = seq.getInt();
                Vector<Action> actVect = nidDependencies.get(new Integer(nid));
                if (actVect == null) {
                    actVect = new Vector<Action>();
                    nidDependencies.put(new Integer(nid), actVect);
                }
                actVect.addElement(action);
                String expr = "PUBLIC _ACTION_" + Integer.toHexString(nid) +
                    " = COMPILE('$_UNDEFINED')";
                try {
                    tree.evaluateData(Data.fromExpr(expr), 0);
                }
                catch (Exception exc) {} //Will always generate an exception since the variable is undefined
                return new IdentData("_ACTION_" + Integer.toHexString(nid));
            }
            catch (Exception exc) {
                System.err.println(
                    "Error in resolving path names in sequential action: " + exc);
                return null;
            }
        }
        if (seq instanceof CompoundData) {
            Data[] descs = ( (CompoundData) seq).getDescs();
            for (int i = 0; i < descs.length; i++)
                descs[i] = traverseSeqExpression(action_data, descs[i]);
        }
        return seq;
    }


    public synchronized void addMonitorListener(MonitorListener monitor)
    {
        monitors.addElement(monitor);
    }

    protected void fireMonitorEvent(Action action, int mode)
    {
        String server;

	System.out.println("----------- fireMonitorEvent SHOT "+shot);

	MonitorEvent event = new MonitorEvent(this, tree, shot,
                                              (curr_phase == null) ? "NONE" :
                                              curr_phase.phase_name, action);

        Enumeration monitor_list = monitors.elements();
        while (monitor_list.hasMoreElements()) {
            MonitorListener curr_listener = (MonitorListener) monitor_list.
                nextElement();
            switch (mode) {
                case MONITOR_BEGIN_SEQUENCE:
                    curr_listener.beginSequence(event);
                    break;
                case MONITOR_BUILD_BEGIN:
                    curr_listener.buildBegin(event);
                    break;
                case MONITOR_BUILD:
                    curr_listener.build(event);
                    break;
                case MONITOR_BUILD_END:
                    curr_listener.buildEnd(event);
                    break;
                case MONITOR_DISPATCHED:
                    curr_listener.dispatched(event);
                    break;
                case MONITOR_DOING:
                    curr_listener.doing(event);
                    break;
                case MONITOR_DONE:
                    curr_listener.done(event);
                    break;
                case MONITOR_END_SEQUENCE:
                    curr_listener.endSequence(event);
                    break;
                case MONITOR_START_PHASE:
                    event.eventId = MonitorEvent.START_PHASE_EVENT;
                    curr_listener.startPhase(event);
                    break;
                case MONITOR_END_PHASE:
                    event.eventId = MonitorEvent.END_PHASE_EVENT;
                    curr_listener.endPhase(event);
                    break;

             }
        }
   }

    void discardAction(Action action)
    {
        action.setStatus(Action.ABORTED, 0, verbose);
        reportDone(action);
    }

    Vector<Integer> getValidSynchSeq(String phaseName, Hashtable<String, Vector<Integer>> currTotSeqNumbers)
    {
        Vector<Integer> currSynchSeqNumbers = synchSeqNumberH.get(phaseName);
        if(currSynchSeqNumbers == null)
            return new Vector<Integer>();
        Vector<Integer> actSynchSeqNumbers = new Vector<Integer>();
        //Get minimum ans maximum sequence number for all servers
        Enumeration<String> serverNames = currTotSeqNumbers.keys();
        int minSeq = 0x7fffffff;
        int maxSeq = -1;
        while(serverNames.hasMoreElements())
        {
            String currServerName = serverNames.nextElement();
            Vector<Integer> currSeqNumbers = currTotSeqNumbers.get(currServerName);
            if(currSeqNumbers.size() > 0)
            {
                int currMin = currSeqNumbers.elementAt(0);
                int currMax = currSeqNumbers.elementAt(currSeqNumbers.size() - 1);
                if(minSeq > currMin)
                    minSeq = currMin;
                if(maxSeq < currMax)
                    maxSeq = currMax;
            }
        }
        if(maxSeq == -1)
            return new Vector<Integer>();//No sequential actions in this phase
        for(int i = 0; i < currSynchSeqNumbers.size(); i++)
        {
            int currSynch = currSynchSeqNumbers.elementAt(i);
            if(currSynch >= minSeq && currSynch < maxSeq)
                actSynchSeqNumbers.addElement(new Integer(currSynch));
        }
        return actSynchSeqNumbers;
    }


    public synchronized boolean startPhase(String phase_name)
    {
         doing_phase = false;
        //increment timestamp. Incoming messages with older timestamp will be ignored
        curr_phase = phases.get(phase_name); //select data structures for the current phase
        if (curr_phase == null)
        {
             curr_phase = new PhaseDescriptor(phase_name);
//           return false; //Phase name does not correspond to any known phase.
        }

        synchSeqNumbers = getValidSynchSeq(phase_name, curr_phase.totSeqNumbers);

        System.out.println("------------- BEGIN PHASE ---------------------  ");
        System.out.print("SYNCHRONOUS SEQUENCE NUMBERS: ");
        for(int i = 0; i < synchSeqNumbers.size(); i++)
            System.out.print(" " + synchSeqNumbers.elementAt(i));
        System.out.println("");
        fireMonitorEvent( (Action)null, MONITOR_START_PHASE);

        //GAB CHRISTMAS 2004

        //dispatch immediate actions, if any
        if (curr_phase.immediate_actions.size() > 0) {
            for (int i = 0; i < curr_phase.immediate_actions.size(); i++) {
                Action action = curr_phase.immediate_actions.elementAt(i);
                if (action.isOn()) {
                    doing_phase = true;
                    dep_dispatched.addElement(action);
                    action.setStatus(Action.DISPATCHED, 0, verbose);
                    fireMonitorEvent(action, MONITOR_DISPATCHED);
                    if(!balancer.enqueueAction(action))
                        discardAction(action);
                }
            }

        }
        //////////////////////
        Enumeration serverClasses = curr_phase.totSeqNumbers.keys();
        //For every server class
        phaseTerminated = new Hashtable<String, Boolean>();
        boolean anyDispatched = false;
        int firstSynch = getFirstValidSynch();
        while(serverClasses.hasMoreElements())
        {
            String serverClass = (String)serverClasses.nextElement();
            Vector<Integer> seqNumbers = curr_phase.totSeqNumbers.get(serverClass);
            int firstSeq;
            try {
                firstSeq = seqNumbers.elementAt(0);
            }catch(Exception exc){firstSeq = -1;}
            Enumeration<Integer> currSeqNumbers = seqNumbers.elements();
            totSeqNumbers.put(serverClass, currSeqNumbers);
        //currSeqNumbers contains the sequence number for the selected phase and for the selected server class

            if (currSeqNumbers.hasMoreElements())
            {
                phaseTerminated.put(serverClass, new Boolean(false));

                 if(firstSynch >= 0 && firstSeq > firstSynch) //Can't start yet
                 {
                    actSeqNumbers.put(serverClass, new Integer(-1));
                    totSeqDispatched.put(serverClass, new Vector<Action>());
                 } else {
                    Integer curr_int = currSeqNumbers.nextElement();
                    actSeqNumbers.put(serverClass, curr_int);
                    Enumeration<Action> first_actions = curr_phase.totSeqActions.get(serverClass).get(curr_int).elements();
                    while (first_actions.hasMoreElements()) {
                        Action action = first_actions.nextElement();
                        if (action.isOn()) {
                            doing_phase = true;
                            Vector<Action> currSeqDispatched = totSeqDispatched.get(serverClass);
                            if(currSeqDispatched == null)
                                totSeqDispatched.put(serverClass, currSeqDispatched = new Vector<Action>());
                            currSeqDispatched.addElement(action);
                            action.setStatus(Action.DISPATCHED, 0, verbose);
                            fireMonitorEvent(action, MONITOR_DISPATCHED);
                            if(!balancer.enqueueAction(action))
                                discardAction(action);
                        }
                    }
                    anyDispatched = true;
                }
            }
        }
        if(anyDispatched)
            return true;
        System.out.println("XXX ------------- END PHASE --------------------- ");
        fireMonitorEvent( (Action)null, MONITOR_END_PHASE);
        return false; //no actions to be executed in this phase
    }

    public void dispatchAction(String actionPath)
    {
        Database currTree = InfoServer.getDatabase();
        NidData nid;
        try {
            nid = currTree.resolve(new PathData(actionPath), 0);
        }catch(Exception exc)
        {
            System.err.println("Cannot resolve " + actionPath);
            return;
        }
        dispatchAction(nid.getInt());

    }


    public synchronized boolean dispatchAction(int nid)
    {
        if (action_nids == null)
            return false;
        Action action = action_nids.get(new Integer(nid));
        if (action == null)
            return false;
        action.setStatus(Action.DISPATCHED, 0, verbose);
        action.setManual(true);
        fireMonitorEvent(action, MONITOR_DISPATCHED);
        if(!balancer.enqueueAction(action))
            discardAction(action);
        return true;
    }

    public /* OCT 2008 synchronized */ void abortAction(int nid)
    {
        if (action_nids == null)
            return;
        Action action = action_nids.get(new Integer(nid));
        if (action == null)
            return;
        balancer.abortAction(action);
    }

    public synchronized void waitPhase()
    {
        try {
            while (doing_phase)
                wait();
        }
        catch (InterruptedException exc) {}
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

    public /*synchronized*/ void actionAborted(ServerEvent event)
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
            String mdsevent = ( (StringData) ( ( (DispatchData) (action.
                getAction().getDispatch())).getCompletion())).getString();
            if (mdsevent != null && !mdsevent.equals("\"\"")) {
                MdsHelper.generateEvent(mdsevent, 0);
            }
        }
        catch (Exception exc) {}

        action.setStatus(Action.DONE, event.getStatus(), verbose);
        fireMonitorEvent(action, MONITOR_DONE);
        reportDone(action);
    }

    protected void reportDone(Action action)
    {
        //remove action from dispatched
        String serverClass = getServerClass(action);
        Vector<Action> currSeqDispatched = totSeqDispatched.get(serverClass);
        if ((currSeqDispatched == null) ||  !currSeqDispatched.removeElement(action))
            dep_dispatched.removeElement(action); //The action belongs only to one of the two
        if(!action.isManual())
        {
            //check dependent actions
            Vector<Action> currDepV = curr_phase.dependencies.get(action.getAction());
            if(currDepV != null && currDepV.size() > 0)
            {
                Enumeration<Action> depend_actions = currDepV.elements();
                while (depend_actions.hasMoreElements())
                {
                    Action curr_action = depend_actions.nextElement();
                    if (curr_action.isOn() &&
                        isEnabled( ( (DispatchData) curr_action.getAction().
                                getDispatch()).getWhen())) { //the dependent action is now enabled
                        dep_dispatched.addElement(curr_action);
                    curr_action.setStatus(Action.DISPATCHED, 0, verbose);
                    fireMonitorEvent(curr_action, MONITOR_DISPATCHED);
                    if(!balancer.enqueueAction(curr_action))
                        discardAction(action);
                    }
                }
            }
            //Handle now possible dependencies based on sequence expression
            Vector<Action> depVect = nidDependencies.get(new Integer(action.getNid()));
            if (depVect != null && depVect.size() > 0) {
                Database tree = InfoServer.getDatabase();
                String doneExpr = "PUBLIC _ACTION_" +
                    Integer.toHexString(action.getNid()) + " = " +
                    action.getStatus();
                try {
                    tree.evaluateData(Data.fromExpr(doneExpr), 0);
                }
                catch (Exception exc) {
                    System.err.println(
                        "Error setting completion TDI variable: " + exc);
                }

                for (int i = 0; i < depVect.size(); i++) {
                    Action currAction = depVect.elementAt(i);
                    try {
                        Data retData = tree.evaluateData( ( (DispatchData)
                            currAction.getAction().
                            getDispatch()).getWhen(), 0);
                        int retStatus = retData.getInt();
                        if ( (retStatus & 0x00000001) != 0) { //Condition satisfied
                            dep_dispatched.addElement(currAction);
                            currAction.setStatus(Action.DISPATCHED, 0, verbose);
                            fireMonitorEvent(currAction, MONITOR_DISPATCHED);
                            if(!balancer.enqueueAction(currAction))
                                discardAction(action);
                        }
                        else { //The action is removed from dep_dispatched since it has not to be executed
                            action.setStatus(Action.ABORTED,
                                             Action.ServerNOT_DISPATCHED,
                                             verbose);
                            fireMonitorEvent(action, MONITOR_DONE);
                        }
                    }
                    catch (Exception exc) {
                        /*     if (exc instanceof DatabaseException && ((DatabaseException)exc).getStatus() == TdiUNKNOWN_VAR) {
                                 currAction.setStatus(Action.ABORTED,
                         Action.ServerINVALID_DEPENDENCY,
                                                  verbose);
                                 fireMonitorEvent(currAction, MONITOR_DONE);
                          }*/
                    }
                }
            }
        }
        if(!isConditional(action))
        {
            if (currSeqDispatched.isEmpty())
            { //No more sequential actions for this sequence number for all server classes
              //Get the list of servers which can advance their sequence number
                    Vector<String> serverClassesV = canProceede(serverClass);
                for(int i = 0; i < serverClassesV.size(); i++)
                {
                    String currServerClass = serverClassesV.elementAt(i);
                    Enumeration<Integer> currSeqNumbers = totSeqNumbers.get(currServerClass);
                    currSeqDispatched = totSeqDispatched.get(currServerClass);
                    if (currSeqNumbers.hasMoreElements())
                    { //Still further sequence numbers
                        Integer curr_int = currSeqNumbers.nextElement();
                        actSeqNumbers.put(currServerClass, curr_int);
                        Enumeration<Action> actions = curr_phase.totSeqActions.get(currServerClass).get(curr_int).elements();
                        while (actions.hasMoreElements()) {
                            Action curr_action = actions.nextElement();
                            currSeqDispatched.addElement(curr_action);
                            curr_action.setStatus(Action.DISPATCHED, 0, verbose); //Spostata da cesare
                            fireMonitorEvent(curr_action, MONITOR_DISPATCHED);
                            if(!balancer.enqueueAction(curr_action))
                                discardAction(action);
    //                    curr_action.setStatus(Action.DISPATCHED, 0, verbose);
                        }
                    }
                    else
                    {
                        phaseTerminated.put(currServerClass, new Boolean(true));
                        if(allTerminatedInPhase())
                        {
                            if (dep_dispatched.isEmpty())
                            { //No more actions at all for this phase
                                doing_phase = false;
            //Report those (dependent) actions which have not been dispatched
                                Enumeration<Action> allActionsEn = curr_phase.all_actions.elements();
                                while(allActionsEn.hasMoreElements())
                                {
                                    Action currAction = allActionsEn.nextElement();
                                    int currDispatchStatus = currAction.getDispatchStatus();
                                    if (currDispatchStatus != Action.ABORTED && currDispatchStatus != Action.DONE)
                                    {
                                        currAction.setStatus(Action.ABORTED,
                                                     Action.ServerCANT_HAPPEN,
                                                     verbose);
                                    //???? Cesare fireMonitorEvent(action, MONITOR_DONE);
                                        fireMonitorEvent(currAction, MONITOR_DONE);
                                 }
                                }
                                System.out.println("------------- FINE DELLA FASE --------------------- ");
                            //fireMonitorEvent(this.curr_phase.phase_name , MonitorEvent.END_PHASE_EVENT);
                                fireMonitorEvent( (Action)null, MONITOR_END_PHASE);
                                synchronized(this)
                                {
                                    notify();
                                }
                                return;
                            }
                        }
                    }
                }
            }
        }
        else //End of conditional action
        {
            if(allTerminatedInPhase())
            {
                if (dep_dispatched.isEmpty())
                { //No more actions at all for this phase
                    doing_phase = false;
//Report those (dependent) actions which have not been dispatched
                    Enumeration<Action> allActionsEn = curr_phase.all_actions.elements();
                    while(allActionsEn.hasMoreElements())
                    {
                        Action currAction = allActionsEn.nextElement();
                        int currDispatchStatus = currAction.getDispatchStatus();
                        if (currDispatchStatus != Action.ABORTED && currDispatchStatus != Action.DONE)
                        {
                            currAction.setStatus(Action.ABORTED,
                                         Action.ServerCANT_HAPPEN,
                                         verbose);
                        //???? Cesare fireMonitorEvent(action, MONITOR_DONE);
                            fireMonitorEvent(currAction, MONITOR_DONE);
                     }
                    }
                    System.out.println("------------- END PHASE --------------------- ");
                //fireMonitorEvent(this.curr_phase.phase_name , MonitorEvent.END_PHASE_EVENT);
                    fireMonitorEvent( (Action)null, MONITOR_END_PHASE);
                    synchronized(this)
                    {
                        notify();
                    }
                    return;
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
        if (when instanceof ConditionData) {
            int modifier = ( (ConditionData) when).getModifier();
            Action action = actions.get(((ConditionData) when).getArgument());
           if(action == null) //Action not present, maybe not enabled
               return false;
            int dispatch_status = action.getDispatchStatus();
            int status = action.getStatus();
            if (dispatch_status != Action.DONE)
                return false;
            switch (modifier) {
                case 0:
                    if ( (status & 1) != 0)
                        return true;
                    return false;
                case ConditionData.IGNORE_UNDEFINED: //???
                case ConditionData.IGNORE_STATUS:
                    return true;
                case ConditionData.NEGATE_CONDITION:
                    if ( (status & 1) == 0)
                        return true;
                    return false;
            }
        }
        if (when instanceof ActionData) {
            Action action = actions.get(when);
            if (action.getDispatchStatus() != Action.DONE)
                return false;
        }
        if (when instanceof DependencyData) {
            Data args[] = ( (DependencyData) when).getArguments();
            if (args.length != 2) {
                System.out.println(
                    "Error: dependency needs 2 arguments. Ignored");
                return false;
            }
            int opcode = ( (DependencyData) when).getOpcode();
            switch (opcode) {
                case DependencyData.DEPENDENCY_AND:
                    return isEnabled(args[0]) && isEnabled(args[1]);
                case DependencyData.DEPENDENCY_OR:
                    return isEnabled(args[0]) || isEnabled(args[1]);
            }
        }
        return true;
    }

    public synchronized void beginSequence(int shot)
    {
        this.shot = shot;
        Enumeration server_list = servers.elements();
        while (server_list.hasMoreElements())
            ( (Server) server_list.nextElement()).beginSequence(shot);
        fireMonitorEvent((Action)null, MONITOR_BEGIN_SEQUENCE);
    }

    public synchronized void endSequence(int shot)
    {
        Enumeration server_list = servers.elements();
        while (server_list.hasMoreElements())
            ( (Server) server_list.nextElement()).endSequence(shot);
        clearTables();
        fireMonitorEvent((Action)null, MONITOR_END_SEQUENCE);
    }

    public synchronized void abortPhase()
    {
        Action action;

        balancer.abort();
        Enumeration serverClasses = totSeqDispatched.keys();
        while(serverClasses.hasMoreElements())
        {
            String serverClass = (String)serverClasses.nextElement();
            Vector<Action> seqDispatched = totSeqDispatched.get(serverClass);
            while (!seqDispatched.isEmpty())
            {
                action = seqDispatched.elementAt(0);
                action.setStatus(Action.ABORTED, 0, verbose);
                seqDispatched.removeElementAt(0);
            }
        }
        while (!dep_dispatched.isEmpty()) {
            action = dep_dispatched.elementAt(0);
            action.setStatus(Action.ABORTED, 0, verbose);
            //seq_dispatched.removeElementAt(0);
        }
    }

    public void redispatchAction(int nid, String phaseName)
    {
        PhaseDescriptor phase = phases.get(phaseName);
        if(doing_phase) //Redispatch not allowed during sequence
            return;
        Action action = phase.all_actions.get(new Integer(nid));
        if(action == null)
        {
            System.err.println("Redispatched a non existent action");
            return;
        }
        dep_dispatched.addElement(action);
        action.setStatus(Action.DISPATCHED, 0, verbose);
        action.setManual(true);
        fireMonitorEvent(action, MONITOR_DISPATCHED);
        if(!balancer.enqueueAction(action))
            discardAction(action);
     }


    public void redispatchAction(int nid)
    {
        if(doing_phase) //Redispatch not allowed during sequence
            return;
        Action action = curr_phase.all_actions.get(new Integer(nid));
        if(action == null)
        {
            System.err.println("Redispatched a non existent action");
            return;
        }
        dep_dispatched.addElement(action);
        action.setStatus(Action.DISPATCHED, 0, verbose);
        action.setManual(true);
        fireMonitorEvent(action, MONITOR_DISPATCHED);
        if(!balancer.enqueueAction(action))
            discardAction(action);
     }


    public void setDefaultServer(Server server)
    {
        balancer.setDefaultServer(server);
    }

    public void setDefaultServerName(String serverName)
    {
        this.defaultServerName = serverName;
    }

    public void startInfoServer(int port)
    {
        System.out.println("Start info server on port " + port);
        (new jDispatcherInfo(port)).start();
    }

    //Inner classes jDispatcherInfo and jDispatcherInfoHandler are used to handle
    //request for information
    class jDispatcherInfoHandler
        extends Thread
    {
        DataInputStream dis;
        DataOutputStream dos;
        jDispatcherInfoHandler(Socket sock)
        {
            try {
                dis = new DataInputStream(sock.getInputStream());
                dos = new DataOutputStream(sock.getOutputStream());
            }
            catch (Exception exc) {
                dis = null;
                dos = null;
            }
        }

        public void run()
        {
            try {
                while (true) {
                    String command = dis.readUTF();
                    if (command.toLowerCase().equals("servers")) {
                        dos.writeInt(servers.size());
                        for (int i = 0; i < servers.size(); i++) {
                            Server currServer = servers.elementAt(i);
                            String serverClass = currServer.getServerClass();
                            String address = "";
                            if (currServer instanceof ActionServer)
                                address = ( (ActionServer) currServer).
                                    getAddress();
                            boolean isActive = currServer.isActive();
                            dos.writeUTF(serverClass);
                            dos.writeUTF(address);
                            dos.writeBoolean(isActive);
                            //dos.writeInt(currServer.getDoingAction());
                            dos.flush();
                        }
                    }
                }
            }
            catch (Exception exc) {}
        }
    }

    class jDispatcherInfo
        extends Thread
    {
        ServerSocket serverSock;
        jDispatcherInfo(int port)
        {
            try {
                serverSock = new ServerSocket(port);
            }
            catch (Exception exc) {
                System.err.println("Error starting jDispatcher info server: " +
                                   exc);
                serverSock = null;
            }
        }

        public void run()
        {
            try {
                while (true) {
                    Socket sock = serverSock.accept();
                    (new jDispatcherInfoHandler(sock)).start();

                }
            }
            catch (Exception exc) {}
        }
    }

    public void disconnected(ServerEvent event)
    {
        System.out.println(" ----- CRASH -------  ");
        fireMonitorEvent(event.getMessage(), MonitorEvent.DISCONNECT_EVENT);
    }

    public void connected(ServerEvent event)
    {
        System.out.println(" ----- RECONNECTED -------  " + event.getMessage() );
        event.getAction();
        fireMonitorEvent(event.getMessage(), MonitorEvent.CONNECT_EVENT);
    }

     protected void fireMonitorEvent(String message, int mode)
    {
        MonitorEvent event = new MonitorEvent(this, mode, message);
        Enumeration monitor_list = monitors.elements();
        while (monitor_list.hasMoreElements()) {
            MonitorListener curr_listener = (MonitorListener) monitor_list.
                nextElement();
            switch (mode) {
                case MonitorEvent.CONNECT_EVENT:
                    curr_listener.connect(event);
                    break;
                case MonitorEvent.DISCONNECT_EVENT:
                    curr_listener.disconnect(event);
                    break;
                case MonitorEvent.START_PHASE_EVENT:
                    curr_listener.startPhase(event);
                    break;
                case MonitorEvent.END_PHASE_EVENT:
                    curr_listener.endPhase(event);
                    break;
            }
        }
   }



    public static void main(String args[])
    {
        Server server;
        Balancer balancer = new Balancer();
        jDispatcher dispatcher = new jDispatcher(balancer);
        dispatcher.addServer(new InfoServer("disp_test"));
        dispatcher.addServer(server = new ActionServer("disp_test",
            "150.178.3.47:8002", "server_1"));
        dispatcher.addServer(new ActionServer("disp_test", "150.178.3.47:8004",
                                              "server_1"));
        dispatcher.addServer(new ActionServer("disp_test", "150.178.3.47:8005",
                                              "server_2"));
        dispatcher.addServer(new ActionServer("disp_test", "150.178.3.47:8006",
                                              "server_2"));
        dispatcher.addServer(new ActionServer("disp_test", "150.178.3.47:8007",
                                              "server_3"));
        dispatcher.addServer(new ActionServer("disp_test", "150.178.3.47:8008",
                                              "server_3"));
        balancer.setDefaultServer(server);
        dispatcher.collectDispatchInformation();
        dispatcher.beginSequence(2);
        dispatcher.startPhase("INITIALIZATION");
        dispatcher.waitPhase();
        dispatcher.endSequence(2);
        System.exit(0);
    }

}
