import java.util.*;
import java.io.*;
import jScope.*;



class jDispatcherIp
    extends MdsIp {
    jDispatcher dispatcher;
    int shot;
    static Vector<Server> servers = new Vector<Server>();
    String treeName;
    String currTreeName;

    Database tree;

    public void setDispatcher(jDispatcher dispatcher) {
        this.dispatcher = dispatcher;
    }

    public jDispatcherIp(int port, jDispatcher dispatcher, String treeName) {
        super(port);
        this.dispatcher = dispatcher;
        this.treeName = treeName;
        System.out.println("Tree name per jDispatcherIp : " + treeName);
        tree = new Database(treeName, -1);
    }

    public MdsMessage handleMessage(MdsMessage[] messages) {
        int ris = -1;
        String command = "", compositeCommand = "";
        compositeCommand = new String(messages[0].body);
        //System.err.println("Comando a jDispatcherIp " + compositeCommand);

        //Handle direct commands: ABORT and DISPATCH
        if (compositeCommand.toUpperCase().startsWith("ABORT") ||
            compositeCommand.toUpperCase().startsWith("DISPATCH") ||
            compositeCommand.toUpperCase().startsWith("REDISPATCH"))
            command = compositeCommand;
        else {
            try {
                StringTokenizer st = new StringTokenizer(compositeCommand, "\"");
                while (! (st.nextToken().equals("TCL")))
                    ;
                st.nextToken();
                command = st.nextToken();
            }
            catch (Exception exc) {
                System.err.println(
                    "Unexpected message has been received by jDispatcherIp:" +
                    compositeCommand + " " + exc);
            }
        }
        try {
            ris = doCommand(command.toUpperCase());
        }
        catch (Exception exc) {
            return new MdsMessage(exc.getMessage(), null);
        }
        if (ris < 0) { //i.e. if any command except get current
            MdsMessage msg = new MdsMessage( (byte) 1);
            msg.status = 1;
            return msg;
        }
        else {
            byte[] buf = new byte[4];
            buf[0] = (byte) ( (ris >> 24) & 0xFF);
            buf[1] = (byte) ( (ris >> 16) & 0xFF);
            buf[2] = (byte) ( (ris >> 8) & 0xFF);
            buf[3] = (byte) (ris & 0xFF);

            MdsMessage msg = new MdsMessage( (byte) 0, (byte) Data.DTYPE_L,
                                            (byte) 0, new int[] {1}
                                            , buf);
            msg.status = 1;
            return msg;
        }

    }

    protected int doCommand(String command) {
        System.out.println("Command: " + command);

        StringTokenizer st = new StringTokenizer(command, "/ ");
        try {
            String first_part = st.nextToken();
            if (first_part.equals("DISPATCH")) {
                String second_part = st.nextToken();
                if (second_part.equals("BUILD"))
                {
                    if(!st.hasMoreTokens())
                        dispatcher.collectDispatchInformation();
                    else
                    {
                        String buildRoot = st.nextToken();
                        dispatcher.collectDispatchInformation(buildRoot);
                    }
                }
                else if (second_part.equals("PHASE")) {
                    String third_part = st.nextToken();
                    dispatcher.startPhase(third_part);
                    dispatcher.waitPhase();
                }
                else {
                    int nid;
                    try {
                        nid = Integer.parseInt(second_part);
                     }
                    catch (Exception ex) {
                        throw new Exception("Invalid command");
                    }
                    if (!dispatcher.dispatchAction(nid))
                        throw new Exception("Cannot dispatch action");
                }
            }
            else if (first_part.equals("CREATE")) {
                String second_part = st.nextToken();
                if (second_part.equals("PULSE")) {
                    shot = Integer.parseInt(st.nextToken());
                    if(shot == 0)
                        shot = getCurrentShot();
                    dispatcher.beginSequence(shot);
                }
                else
                    throw new Exception("Invalid Command");
            }
            else if (first_part.equals("SET")) {
                String second_part = st.nextToken();
                if (second_part.equals("TREE"))
                {
		    if(st.hasMoreTokens())
		    {
			currTreeName = st.nextToken();
		    }

		    try {

			if(st.hasMoreTokens())
		        {
			    shot = Integer.parseInt(st.nextToken());

			    //setCurrentShot(shot);
			    dispatcher.setTree(currTreeName, shot);
			} else {
			    dispatcher.setTree(currTreeName);
			}

		     }
		     catch(Exception exc)
		     {
			System.err.println("Wrong shot number in SET TREE command\n" + exc);
		     }

                }
                else if (second_part.equals("CURRENT")) {
                    String third_part = st.nextToken();
                    setCurrentShot(Integer.parseInt(third_part));
                }
                else
                    throw new Exception("Invalid Command");
            }
            else if (first_part.equals("CLOSE")) {
                dispatcher.endSequence(shot);
            }
            else if (first_part.equals("ABORT")) {
                String second_part = st.nextToken();
                int nid;
                try {
                    if(second_part.toUpperCase().equals("PHASE"))
                        dispatcher.abortPhase();
                    else
                    {
                        nid = Integer.parseInt(second_part);
                        dispatcher.abortAction(nid);
                    }
                 }
                catch (Exception ex) {
                    throw new Exception("Invalid command");
                }
            }
            else if (first_part.equals("REDISPATCH")) {
                String second_part = st.nextToken();
                String phaseName = null;
                try {
                    phaseName = st.nextToken();
                }catch(Exception exc){phaseName = null;}
                int nid;
                try {
                    nid = Integer.parseInt(second_part);
                }
                catch (Exception ex) {
                    throw new Exception("Invalid command");
                }
                if (phaseName != null)
                    dispatcher.redispatchAction(nid, phaseName);
                else
                    dispatcher.redispatchAction(nid);
            }
            else if (first_part.equals("GET")) {
                String second_part = st.nextToken();
                if (second_part.equals("CURRENT")) {
                    System.out.println("Current shot :" + getCurrentShot());
                    return getCurrentShot();
                }
                else
                    throw new Exception("Invalid Command");
            }
            else if (first_part.equals("INCREMENT")) {
                String second_part = st.nextToken();
                if (second_part.equals("CURRENT")) {
                    incrementCurrentShot();
                }
                else
                    throw new Exception("Invalid Command");
            }
            else if (first_part.equals("CHECK"))
            {
                if(dispatcher.checkEssential())
                    return 1;
                else
                    return 0;
            }
            else if (first_part.equals("DO"))
            {
                try {
                    String second_part = st.nextToken();
                    dispatcher.dispatchAction(second_part);
                }catch(Exception exc) {
                    throw new Exception("Invalid Command");
                }
            }
            else
                throw new Exception("Invalid Command");
            return -1;
        }
        catch (Exception exc) {
            System.out.println("Invalid command: " + command);
            return -1;
        }
    }

    int getCurrentShot() {
        try {
            return tree.getCurrentShot(treeName);
        }
        catch (Exception exc) {
            return -1;
        }
    }

    void setCurrentShot(int shot) {
        try {
            tree.setCurrentShot(currTreeName, shot);
        }
        catch (Exception exc) {}
    }

    void incrementCurrentShot() {
        try {
            int shot = tree.getCurrentShot(treeName);
            tree.setCurrentShot(shot + 1);
        }
        catch (Exception exc) {
            System.err.println("Error incrementing current shot");
        }
    }

    public static void main(String args[]) {
        String treeName;
        if (args.length >= 1)
            treeName = args[0];
        else
            treeName = "RFX";

		Properties properties = MdsHelper.initialization(treeName);
		if( properties == null )
		  System.exit(0);

        Balancer balancer = new Balancer();
        jDispatcher dispatcher = new jDispatcher(balancer);
		dispatcher.addServer(new InfoServer());

        int port = 0;
        try {
            port = Integer.parseInt(properties.getProperty("jDispatcher.port"));
        }
        catch (Exception exc) {
            System.out.println("Cannot read port");
            System.exit(0);
        }

        int info_port = 0;
        try {
            info_port = Integer.parseInt(properties.getProperty("jDispatcher.info_port"));
            dispatcher.startInfoServer(info_port);
        }
        catch (Exception exc) {
            System.out.println("Cannot read info_port");
            System.exit(0);
        }

        System.out.println("Start dispatcher on port " + port);
        jDispatcherIp dispatcherIp = new jDispatcherIp(port, dispatcher,
            treeName);
        dispatcherIp.start();

        int i = 1;
        while (true) {
            String server_class = properties.getProperty("jDispatcher.server_" +
                i +
                ".class");
            if (server_class == null)
                break;
            String server_ip = properties.getProperty("jDispatcher.server_" + i +
                ".address");
            if (server_ip == null)
                break;
            String server_subtree = properties.getProperty(
                "jDispatcher.server_" +
                i + ".subtree");
            boolean useJavaServer;
            try {
                useJavaServer = properties.getProperty("jDispatcher.server_" + i +
                ".use_jserver").equals("true");
            }catch(Exception exc){useJavaServer = true;}
            int watchdogPort;
            try {
                watchdogPort = Integer.parseInt(properties.getProperty("jDispatcher.server_" + i +
                ".watchdog_port"));
            }catch(Exception exc){watchdogPort = -1;}

            Server server = new ActionServer("", server_ip.trim(),
                                             server_class.trim(),
                                             server_subtree, useJavaServer, watchdogPort);

            servers.addElement(server);
            dispatcher.addServer(server);
            i++;
        }
        i = 1;

        while (true) {
            String monitor_port = properties.getProperty("jDispatcher.monitor_" +
                i +
                ".port");
            if (monitor_port == null)
                break;
            try {
                int monitor_port_int = Integer.parseInt(monitor_port);
                System.out.println("Start monitor server on port " + monitor_port_int);
                MdsMonitor monitor = new MdsMonitor(monitor_port_int);
                dispatcher.addMonitorListener(monitor);
                monitor.start();
                i++;
            }
            catch (Exception exc) {
                break;
            }
        }
/*
        String actionsMonitorPort = properties.getProperty("jDispatcher.actions_monitor_port");
        if (actionsMonitorPort != null)
        {
        try {
            int actionsMonitorPortVal = Integer.parseInt(actionsMonitorPort);
            MdsActionsMonitor actionsMonitor = new MdsActionsMonitor(actionsMonitorPortVal);
            dispatcher.addMonitorListener(actionsMonitor);
            actionsMonitor.start();
            }
            catch (Exception exc) {}
            System.out.println("Start done actions monitor on port : " + actionsMonitorPort);
        }
*/
        String default_server = properties.getProperty(
            "jDispatcher.default_server_idx");
        try {
            int default_server_idx = Integer.parseInt(default_server) - 1;
            Server server = servers.elementAt(default_server_idx);
            dispatcher.setDefaultServer(server);
        }
        catch (Exception exc) {}

        /*
             jDispatcherIp dispatcherIp = new jDispatcherIp(port, dispatcher, treeName);
                dispatcherIp.start();
         */

        i = 1;
        while (true)
        {
            String phaseName = properties.getProperty("jDispatcher.phase_" + i + ".name");
            if (phaseName == null)
                break;
            Vector<Integer> currSynchNumbers = new Vector<Integer>();
            String currSynchStr = properties.getProperty("jDispatcher.phase_" + i + ".synch_seq_numbers");
            if(currSynchStr != null)
            {
                StringTokenizer st = new StringTokenizer(currSynchStr, " ,");
                while(st.hasMoreTokens())
                {
                    try {
                        currSynchNumbers.addElement(new Integer(st.nextToken()));
                    }catch(Exception exc)
                    {
                        System.out.println("WARNING: Invalid Syncronization number for phase " + phaseName);
                    }
                }
                dispatcher.addSynchNumbers(phaseName, currSynchNumbers);
             }
            i++;
         }
         try {
            dispatcherIp.getListenThread().join();
        }
        catch (Exception exc) {}
    }
}
