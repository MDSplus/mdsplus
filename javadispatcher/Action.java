import java.util.*;

class Action 
{
    ActionData action;
    int nid;
    String name;
    boolean on;
    int dispatch_status;
    int status;
    int timestamp;
    boolean manual = false;
    static final int NOT_DISPATCHED = 1;
    static final int DISPATCHED = 2;
    static final int DOING = 3;
    static final int DONE = 4;
    static final int ABORTED = 5;
    
    public Action(ActionData action, int nid, String name, boolean on)
    {
        this.action = action;
        this.nid = nid;
        this.name = name;
        this.on = on;
        dispatch_status = NOT_DISPATCHED;
        status = 0;
        
    }
    
   // public int getTimestamp() {return timestamp; }
    public void setTimestamp(int timestamp) {this.timestamp = timestamp; }
    public ActionData getAction() {return action; }
    public int getNid() {return nid; }
    public String getName() {return name; }
    public boolean isOn() {return on; }
    public int getDispatchStatus() {return dispatch_status; }
    public int getStatus() {return status; }
    public boolean isManual() {return manual; }
    public void setManual(boolean manual) {this.manual = manual; }
    void setStatus(int status) {this.status = status; }
   
    void setStatus(int dispatch_status, int status, boolean verbose) 
    {
        String server;
        this.status = status;
        this.dispatch_status = dispatch_status;
        
        if(verbose)
        {
            try {
                server = ((DispatchData)action.getDispatch()).getIdent().getString();
            } catch(Exception e) {server = ""; }
            switch(dispatch_status) {
                case DISPATCHED : System.out.println(""+ new Date() + " Dispatching node " +
                    name + " to " + server); break;
                 case DOING : System.out.println(""+new Date() + " " +server + 
                        " is beginning action " + name); break;
                 case DONE : 
                        if((status & 1) != 0)
                            System.out.println(""+new Date() + " Action " +name + " completed  ");
                        else
                            System.out.println(""+new Date() + " Action " +name + 
                                " failed  " + MdsHelper.getErrorString(status)); 
                        break;
                 case ABORTED: System.out.println(""+new Date() + " Action " +name + " aborted"); break;
            }      
        }   
    }
    public String toString() {return name; }  
    
}