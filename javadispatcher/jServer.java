import  java.net.*;
import java.io.*;
import java.util.*;



public class jServer extends MdsIp {
  static final int SrvNoop        =0;           /**** Used to start server ****/
  static final int SrvAbort       =1;           /**** Abort current action or mdsdcl command ***/
  static final int SrvAction      =2;           /**** Execute an action nid in a tree ***/
  static final int SrvClose       =3;           /**** Close open trees ***/
  static final int SrvCreatePulse =4;           /**** Create pulse files for single tree (nosubtrees) ***/
  static final int SrvSetLogging  =5;           /**** Turn logging on/off ***/
  static final int SrvCommand     =6;           /**** Execute MDSDCL command ***/
  static final int SrvMonitor     =7;           /**** Broadcast messages to action monitors ***/
  static final int SrvShow        =8;           /**** Request current status of server ***/
  static final int SrvStop        =9;           /**** Stop server ***/
  static final int SrvRemoveLast  =10;         /**** Remove last item in queue if action pending ***/

  static final int SrvJobBEFORE_NOTIFY =1;
  static final int SrvJobAFTER_NOTIFY  =2;
  static final int SrvJobABORTED     =1;
  static final int SrvJobSTARTING    =2;
  static final int SrvJobFINISHED    =3;

  static final int ServerABORT    = 0xfe18032;

  public static int doingNid;

  Vector retSocketsV = new Vector();
  Database mdsTree = null;
  ActionQueue actionQueue = new ActionQueue();
  Worker worker = new Worker();

//static inner class ActionDescriptor is used to keep action-related information
  static class ActionDescriptor
  {
    NidData nid;
    InetAddress address;
    int port;
    int id;
    String tree;
    int shot;
    ActionDescriptor(NidData nid, InetAddress address, int port, int id,
                     String tree, int shot)
    {
      this.nid = nid;
      this.address = address;
      this.port = port;
      this.id = id;
      this.tree = tree;
      this.shot = shot;
    }
    final NidData getNid(){return nid;}
    final InetAddress getAddress(){return address;}
    final int getPort() {return port;}
    final int getId() {return id;}
    final String getTree() { return tree;}
    final int getShot(){return shot;}
  }

//Inner class ActionQueue keeps a queue of ActionDesctor objects and manages synchronization
  class ActionQueue
  {
    Vector actionV = new Vector();
    synchronized void enqueueAction(ActionDescriptor actionDescr)
    {
      actionV.addElement(actionDescr);
      notify();
    }

    synchronized ActionDescriptor nextAction()
    {
      while(actionV.size() == 0)
      {
        try{
          wait();
        }catch(InterruptedException exc){}
      }
      ActionDescriptor retAction = (ActionDescriptor)actionV.elementAt(0);
      actionV.removeElementAt(0);
      return retAction;
    }
  }


  //Inner class Worker performs assigned computation on a separate thread.
  //It gets the nid to be executed from an instance of nidQueue, and executes
  //he action on a separate thread until either the timeout is reached (is specified)
  //or an abort command is received
  class Worker extends Thread
  {
      ActionDescriptor currAction;
      int retStatus = 0;
      boolean currentActionAborted = false;


      public synchronized void awakeWorker()
      {
        notify();
      }

      public synchronized void abortCurrentAction()
      {
        currentActionAborted = true;
        notify();
      }

      public void run()
      {
        while(true)
        {
          currAction = actionQueue.nextAction();
          NidData nid = currAction.getNid();
          String message = "" + currAction.getId() + " " + SrvJobSTARTING + " 1 0";
          writeAnswer(currAction.getAddress(), currAction.getPort(), message);
          ActionMaker currMaker;
          synchronized(this)
          {
            currentActionAborted = false;

            currMaker = new ActionMaker(currAction);
            currMaker.start();
            try {
              wait(); //until either the action terminates or timeout expires or an abort is received
            }catch(InterruptedException exc){}
          }
          if(!currentActionAborted)
          {
            //String answer = "" + currAction.getId() + " " + SrvJobFINISHED + " 1 0";
            String answer = "" + currAction.getId() + " " + SrvJobFINISHED + " " +
                retStatus + " 0";
            writeAnswer(currAction.getAddress(), currAction.getPort(), answer);
          }
          else
          {
            currMaker.setAborted();
            String answer = "" + currAction.getId() + " " + SrvJobABORTED + " 1 0";
            writeAnswer(currAction.getAddress(), currAction.getPort(), answer);
          }
        }
      }
      //Inner class ActionMaker executes the action and records whether such action has been aborted
      class ActionMaker
          extends Thread {
          boolean aborted = false;
          ActionDescriptor action;
          ActionMaker(ActionDescriptor action) {
              this.action = action;
          }

          public void run() {
               int status = doSimpleAction(action.getNid(), action.getTree(),
                                          action.getShot());
              if (!aborted) {
                  retStatus = status;
                  awakeWorker();
              }
          }

          public void setAborted() {
              aborted = true;
          }
      }//End ActionMaker class implementation


  } //End Worker class impementation

/////jServer class implementation

  jServer(int port)
  {
    super(port);
    worker.start();
  }

  synchronized Socket getRetSocket(InetAddress ip, int port)
  {
    for(int i = 0; i < retSocketsV.size(); i++)
    {
      Socket currSock = (Socket)retSocketsV.elementAt(i);
      if(currSock.getInetAddress().equals(ip) && currSock.getPort()==port)
        return currSock;
    }
    try {

      Socket newSock = new Socket(ip, port);
      retSocketsV.addElement(newSock);
      return newSock;
    }catch(Exception exc){return null;}
  }

  synchronized void removeRetSocket(Socket sock)
  {
    retSocketsV.remove(sock);
  }



  public MdsMessage handleMessage(MdsMessage [] messages)
  {
    int ris = -1;
    int status = 1;
    String command = "", compositeCommand = "";
    try {
      command = new String(messages[0].body);
      //System.out.println("Command: " + command);
      if(command.toLowerCase().startsWith("kill"))
      {
        Timer timer = new Timer();
        timer.schedule(new TimerTask() {
          public void run() {System.exit(0); }}, 500);
      }


      if (command.startsWith("ServerQAction")) {
        InetAddress address = InetAddress.getByAddress(messages[1].body);
        DataInputStream dis = new DataInputStream(new ByteArrayInputStream(
            messages[2].body));
        int port = dis.readShort();
        dis = new DataInputStream(new ByteArrayInputStream(messages[3].body));
        int operation = dis.readInt();
        dis = new DataInputStream(new ByteArrayInputStream(messages[4].body));
        int flags = dis.readInt();
        dis = new DataInputStream(new ByteArrayInputStream(messages[5].body));
        int id = dis.readInt();
        switch (operation) {
          case SrvAction:
            String tree = new String(messages[6].body);
            dis = new DataInputStream(new ByteArrayInputStream(messages[7].
                body));
            int shot = dis.readInt();
            dis = new DataInputStream(new ByteArrayInputStream(messages[8].
                body));
            int nid = dis.readInt();

          //  System.out.println("SrvAction " + id + " " + tree + " " + shot +
          //                     " " + nid);
            actionQueue.enqueueAction(new ActionDescriptor(new NidData(nid), address, port, id,
                tree, shot));
            break;

          case SrvAbort:
            dis = new DataInputStream(new ByteArrayInputStream(
                messages[6].body));
            int flushInt = dis.readInt();
                boolean flush = (flushInt != 0);
           // System.out.println("SrvAbort " + id + " " + flush);
            worker.abortCurrentAction();
            //answer = "" + id + " " + SrvJobABORTED + " 1 0";
            //writeAnswer(address, port, answer);
            break;
          case SrvClose:
            //System.out.println("SrvClose " + id);
            String answer = "" + id + " " + SrvJobFINISHED + " 1 0";
            if(mdsTree != null)
            {
              try {
                mdsTree.close(0);
                mdsTree = null;
              }catch(Exception exc){mdsTree = null;}
            }

            writeAnswer(address, port, answer);
            break;
          case SrvCreatePulse:
            tree = new String(messages[6].body);
            dis = new DataInputStream(new ByteArrayInputStream(messages[10].
                body));
            shot = dis.readInt();

            //System.out.println("SrvCreatePulse " + id + " " + tree + " " + shot);
            answer = "" + id + " " + SrvJobFINISHED + " 1 0";
            writeAnswer(address, port, answer);
            break;
          case SrvCommand:
            String cli = new String(messages[6].body);
            command = new String(messages[7].body);
            //System.out.println("SrvCommand " + id + " " + cli + " " + command);
            break;
          case SrvSetLogging:
            //System.out.println("SrvSetLogging " + id);
            break;
          case SrvStop:
            //System.out.println("SrvStop " + id);
            break;
          default:
            System.out.println("Unknown Operation: " + operation);
        }
      }
    }
    catch (Exception exc) {
      System.err.println(exc);
    }
    MdsMessage msg = new  MdsMessage((byte)0, Descriptor.DTYPE_LONG,
                        (byte)0,
                        (int[])null,
                        new byte[]{(byte)0,(byte)0,(byte)0,(byte)0 });


   /* public MdsMessage(byte descr_idx, byte dtype,
                    byte nargs,
                    int dims[],
                    byte body[])*/

    msg.status = 1;
    return msg;
  }

  synchronized void writeAnswer(InetAddress ip, int port, String answer)
  {

   // System.out.println("Answer: " + answer);
    try {
      DataOutputStream dos = new DataOutputStream(getRetSocket(ip, port).getOutputStream());
      byte[] answerMsg = answer.getBytes();
      byte[] retMsg = new byte[60];
      for(int i = 0; i < answerMsg.length; i++)
        retMsg[i] = answerMsg[i];
      for(int i = answerMsg.length; i < 60; i++)
        retMsg[i] = (byte)0;
      dos.write(retMsg);
      dos.flush();
    }catch(Exception exc)
    {
      System.err.println("Error sending answer: " + exc);
    }

  }

  public static void main(String args[]) {
    int port;
    try {
      System.out.println(args[0]);
      port = Integer.parseInt(args[0]);
    }
    catch (Exception exc) {
      port = 8002;
    }

    if(args.length > 1)
    {
      String tclBatch = args[1];
      Database tree = new Database();
      try {
        tree.evaluateData(Data.fromExpr("tcl(\'@"+tclBatch+"\')"), 0);
      }catch(Exception exc)
      {
        System.err.println("Error executing initial TCL batch: " + exc);
      }
    }


    jServer server = new jServer(port);
    server.start();

    BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
    try {
      while(true)
      {
        String command = br.readLine();
        if (command.equals("exit")) {
          server.closeAll();
          System.exit(0);
        }
      }
    }catch(Exception exc){}

  }


  String lastTree = null;
  int lastShot;
//Execute the action. Return the action status.
  int doSimpleAction(NidData nid,  String tree, int shot)
  {
    int status;
    String name = "";
    try {
      if (mdsTree == null || !tree.equals(lastTree) || shot != lastShot)
      {
        if(mdsTree != null)
          mdsTree.close(0);
        mdsTree = new Database(tree, shot);
        mdsTree.open();
        lastTree = tree;
        lastShot = shot;
      }
      doingNid = nid.getInt();
      try {
        Data.evaluate("DevSetDoingNid(" + doingNid + ")");
      }catch(Exception exc){}
      try {
        name = mdsTree.getInfo(nid, 0).getFullPath();
      }
      catch (Exception exc) {
        System.err.println("Cannot resolve action name");
        name = "";
      }

      System.out.println("" + new Date() + ", Doing " + name + " in " +
                         tree + " shot " + shot);

      mdsTree.doAction(nid, 0);
      System.out.println(""+new Date()+ ", Done " + name + " in " + tree + " shot " + shot);
      status = 1;
    }catch(Exception exc)
    {
      System.out.println(""+new Date()+ ", Failed " + name + " in " + tree + " shot " + shot + ": " + exc);
      if(exc instanceof DatabaseException)
        status = ((DatabaseException)exc).getStatus();
      else
      status = 0;
    }
    return status;
  }

  public void closeAll()
  {
    for(int i = 0; i < retSocketsV.size(); i++)
    {
      Socket currSock = (Socket)retSocketsV.elementAt(i);
      if(currSock != null)
      {
        try {
          currSock.shutdownInput();
          currSock.shutdownOutput();
          currSock.close();
        }catch(Exception exc){}
      }
    }
  }

}
