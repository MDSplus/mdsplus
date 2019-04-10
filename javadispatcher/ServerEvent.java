import java.util.*;

class ServerEvent extends EventObject
{
    Action action;
    String message;
    public ServerEvent(Object obj, Action action)
    {
        super(obj);
        this.action = action;
    }

    public ServerEvent(Object obj, String message)
    {
        super(obj);
        this.message = message;
    }

    public Action getAction() {return action; }
    public int getStatus() {return action.getStatus();}
    public String getMessage() {return message;}

    //public int getTimestamp() {return action.getTimestamp();}
}