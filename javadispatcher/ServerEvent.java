import java.util.*;

class ServerEvent extends EventObject
{
    Action action;
    public ServerEvent(Object obj, Action action)
    {
        super(obj);
        this.action = action;
    }
    public Action getAction() {return action; }
    public int getStatus() {return action.getStatus();}
    //public int getTimestamp() {return action.getTimestamp();}
}