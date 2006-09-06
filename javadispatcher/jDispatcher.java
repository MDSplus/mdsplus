import java.util.*;
import java.io.*;
import java.net.*;

class jDispatcher
    implements ServerListener
{
    static boolean verbose = true;

    static final int MONITOR_BUILD_BEGIN = 1;
    static final int MONITOR_BUILD = 2;
    static final int MONITOR_BUILD_END = 3;
    static final int MONITOR_DISPATCHED = 5;
    static final int MONITOR_DOING = 6;
    static final int MONITOR_DONE = 7;


    static final int TdiUNKNOWN_VAR = 0xfd380f2;

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

    Hashtable nidDependencies = new Hashtable();
    /**
             Indexed by nid. For each nid, a vector of potentially dependent actions is defined.
             Note that the hierarchy of ActionData does not override equals and hashCode, and therefore
             two actions are equal only if ther refer to the same instance.
             This kind of dependency is NOT restricted to the current phase.
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


        Vector immediate_actions = new Vector();
        /**
                 List of conditional actions with no dependency. These actions will be started immediately
                 at the beginning of the phase
         */

        Hashtable all_actions = new Hashtable();
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
        try {
            jbInit();
        }
        catch (Exception ex) {
            ex.printStackTrace();
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
        seq_dispatched.removeAllElements();
        nidDependencies.clear();
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
        while (server_list.hasMoreElements())
            ( (Server) server_list.nextElement()).setTree(tree);
    }

    public void setTree(String tree, int shot)
    {
        this.tree = tree;
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

    protected void insertAction(Action action, boolean is_first,
                                boolean is_last)
    {
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

            curr_phase = (PhaseDescriptor) phases.get(phase_name);
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

                if (isSequenceNumber) {
                    Integer seq_obj = new Integer(seq_number);
                    if (curr_phase.seq_actions.containsKey(seq_obj)) {
                        ( (Vector) curr_phase.seq_actions.get(seq_obj)).
                            addElement(
                                action);
                    }
                    else { //it is the first time such a sequence number is referenced
                        Vector curr_vector = new Vector();
                        curr_vector.add(action);
                        curr_phase.seq_actions.put(seq_obj, curr_vector);
                        int size = curr_phase.seq_numbers.size();
                        if (seq_number >= size) {
                            for (int i = size; i < seq_number; i++)
                                curr_phase.seq_numbers.addElement(new Integer( -
                                    1));
                            curr_phase.seq_numbers.addElement(seq_obj);
                        }
                        else {
                            if ( ( (Integer) curr_phase.seq_numbers.elementAt(
                                seq_number)).intValue() == -1) {
                                curr_phase.seq_numbers.removeElementAt(
                                    seq_number);
                                curr_phase.seq_numbers.insertElementAt(seq_obj,
                                    seq_number);
                            }
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

            curr_phase.dependencies.put(action.getAction(), new Vector()); //Insert every new action in dependencies hashtable
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
            PhaseDescriptor currPhase = (PhaseDescriptor) phases.get(phaseNames.
                nextElement());
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
            Vector act_vector = (Vector) currPhase.dependencies.get( ( (
                ConditionData) when).getArgument());
            if (act_vector != null &&
                (action = (Action) actions.get(action_data)) != null)
                act_vector.addElement(action);
            else
                System.out.println(
                    "Warning: condition does not refer to a known action");
        }
        else if (when instanceof ActionData) {
            Vector act_vector = (Vector) currPhase.dependencies.get(when);
            if (act_vector != null &&
                (action = (Action) actions.get(action_data)) != null)
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
        Action action = (Action) actions.get(action_data);
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
                    nid = ( (NidData) tree.resolve( (PathData) seq, 0)).getInt();
                else
                    nid = seq.getInt();
                Vector actVect = (Vector) nidDependencies.get(new Integer(nid));
                if (actVect == null) {
                    actVect = new Vector();
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

    protected synchronized void fireMonitorEvent(Action action, int mode)
    {
        String server;
        MonitorEvent event = new MonitorEvent(this, tree, shot,
                                              (curr_phase == null) ? "NONE" :
                                              curr_phase.phase_name, action);
        Enumeration monitor_list = monitors.elements();
        while (monitor_list.hasMoreElements()) {
            MonitorListener curr_listener = (MonitorListener) monitor_list.
                nextElement();
            switch (mode) {
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
            }
        }
   }

    void discardAction(Action action)
    {
        action.setStatus(Action.ABORTED, 0, verbose);
        reportDone(action);
    }

    public synchronized boolean startPhase(String phase_name)
    {
        doing_phase = false;
        //increment timestamp. Incoming messages with older timestamp will be ignored
        curr_phase = (PhaseDescriptor) phases.get(phase_name); //select data structures for the current phase
        if (curr_phase == null)
            return false; //Phase name does not correspond to any known phase.

        //GAB CHRISTMAS 2004

        //dispatch immediate actions, if any
        if (curr_phase.immediate_actions.size() > 0) {
            for (int i = 0; i < curr_phase.immediate_actions.size(); i++) {
                Action action = (Action) curr_phase.immediate_actions.elementAt(
                    i);
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
        curr_seq_numbers = curr_phase.seq_numbers.elements();
        //curr_seq_numbers contains the sequence number for the selected phase

        if (curr_seq_numbers.hasMoreElements()) {
            Integer curr_int;
            do {
                curr_int = (Integer) curr_seq_numbers.nextElement();
            }
            while (curr_int.intValue() == -1);
            Vector first_action_vect = (Vector) curr_phase.seq_actions.get(
                curr_int);
            Enumeration first_actions = first_action_vect.elements();
            while (first_actions.hasMoreElements()) {
                Action action = (Action) first_actions.nextElement();
                if (action.isOn()) {
                    doing_phase = true;
                    seq_dispatched.addElement(action);
                    action.setStatus(Action.DISPATCHED, 0, verbose);
                    fireMonitorEvent(action, MONITOR_DISPATCHED);
                    if(!balancer.enqueueAction(action))
                        discardAction(action);

                }
            }
            return true;
        }
        return false; //no actions to be executed in this phase
    }

    public synchronized boolean dispatchAction(int nid)
    {
        if (action_nids == null)
            return false;
        Action action = (Action) action_nids.get(new Integer(nid));
        if (action == null)
            return false;
        action.setStatus(Action.DISPATCHED, 0, verbose);
        action.setManual(true);
        fireMonitorEvent(action, MONITOR_DISPATCHED);
        if(!balancer.enqueueAction(action))
            discardAction(action);
        return true;
    }

    public synchronized void abortAction(int nid)
    {
        if (action_nids == null)
            return;
        Action action = (Action) action_nids.get(new Integer(nid));
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

System.out.println("ACTION ABORTED 1");
        event.getAction().setStatus(Action.ABORTED, 0, verbose);
System.out.println("ACTION ABORTED 2");
        fireMonitorEvent(event.getAction(), MONITOR_DONE);
System.out.println("ACTION ABORTED 3");
        reportDone(event.getAction());
System.out.println("FINE ACTION ABORTED");
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
        if (!seq_dispatched.removeElement(action))
            dep_dispatched.removeElement(action); //The action belongs only to one of the two
        if(!action.isManual())
        {
            //check dependent actions
            Enumeration depend_actions = ( (Vector) curr_phase.dependencies.get(
                action.getAction())).elements();
            while (depend_actions.hasMoreElements()) {
                Action curr_action = (Action) (depend_actions.nextElement());
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
            //Handle now possible dependencies based on sequence expression
            Vector depVect = (Vector) nidDependencies.get(new Integer(action.
                getNid()));
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
                    Action currAction = (Action) depVect.elementAt(i);
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
        if (seq_dispatched.isEmpty()) { //No more sequential actions for this sequence number
            if (curr_seq_numbers.hasMoreElements()) { //Still further sequence numbers
                Integer curr_int;
                do {
                    curr_int = (Integer) curr_seq_numbers.nextElement();
                }
                while (curr_int.intValue() == -1);

                Vector first_action_vect = (Vector) curr_phase.seq_actions.get(
                    curr_int);
                Enumeration actions = first_action_vect.elements();
                while (actions.hasMoreElements()) {
                    Action curr_action = (Action) actions.nextElement();
                    seq_dispatched.addElement(curr_action);
                    curr_action.setStatus(Action.DISPATCHED, 0, verbose); //Spostata da cesare
                    fireMonitorEvent(curr_action, MONITOR_DISPATCHED);
                    if(!balancer.enqueueAction(curr_action))
                        discardAction(action);
//                    curr_action.setStatus(Action.DISPATCHED, 0, verbose);
                }
            }
            else {
                if (dep_dispatched.isEmpty()) { //No more actions at all for this phase
                    doing_phase = false;
    //Report those (dependent) actions which have not been dispatched
                    Enumeration allActionsEn = curr_phase.all_actions.elements();
                    while(allActionsEn.hasMoreElements())
                    {
                        Action currAction = (Action) allActionsEn.nextElement();
                        int currDispatchStatus = currAction.getDispatchStatus();
                        if (currDispatchStatus != Action.ABORTED && currDispatchStatus != Action.DONE)
                        {
                            action.setStatus(Action.ABORTED,
                                             Action.ServerCANT_HAPPEN,
                                             verbose);
                            fireMonitorEvent(action, MONITOR_DONE);
                        }
                    }
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
        if (when instanceof ConditionData) {
            int modifier = ( (ConditionData) when).getModifier();
            Action action = (Action) actions.get( ( (ConditionData) when).
                                                 getArgument());
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
            Action action = (Action) actions.get(when);
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
    }

    public synchronized void endSequence(int shot)
    {
        Enumeration server_list = servers.elements();
        while (server_list.hasMoreElements())
            ( (Server) server_list.nextElement()).endSequence(shot);
        clearTables();
    }

    public synchronized void abortPhase()
    {
        Action action;

        balancer.abort();
        while (!seq_dispatched.isEmpty()) {
            action = (Action) seq_dispatched.elementAt(0);
            action.setStatus(Action.ABORTED, 0, verbose);
            seq_dispatched.removeElementAt(0);
        }
        while (!dep_dispatched.isEmpty()) {
            action = (Action) dep_dispatched.elementAt(0);
            action.setStatus(Action.ABORTED, 0, verbose);
            seq_dispatched.removeElementAt(0);
        }
    }


    public void redispatchAction(int nid)
    {
        if(doing_phase) //Redispatch not allowed during sequence
            return;
        Action action = (Action)curr_phase.all_actions.get(new Integer(nid));
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
                            Server currServer = (Server) servers.elementAt(i);
                            String serverClass = currServer.getServerClass();
                            String address = "";
                            if (currServer instanceof ActionServer)
                                address = ( (ActionServer) currServer).
                                    getAddress();
                            boolean isActive = currServer.isActive();
                            dos.writeUTF(serverClass);
                            dos.writeUTF(address);
                            dos.writeBoolean(isActive);
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

    private void jbInit() throws Exception
    {
    }

}
